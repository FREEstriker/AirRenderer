#include "MeshRenderer.h"
#include "Configuration.h"
#include <QDebug>
#include <qdir.h>
#include <QCoreApplication>
#include "FaceContext.h"
#include "PixelInContext.h"
#include "PixelOutContext.h"
#include "PixelIterator.h"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include "VertexInContext.h"
#include "VertexOutContext.h"
#include "FaceContext.h"


MeshRenderer::MeshRenderer():MeshRenderer("../../Model/Flat_Wall_Normal.ply")
{

}
MeshRenderer::MeshRenderer(std::string filePath):Component("MeshRenderer")
{
	std::string s = (QCoreApplication::applicationDirPath() + "/" + QString::fromStdString(filePath)).toStdString();
    OpenMesh::IO::Options o = OpenMesh::IO::Options::ColorFloat + OpenMesh::IO::Options::VertexColor + OpenMesh::IO::Options::ColorAlpha + OpenMesh::IO::Options::VertexTexCoord + OpenMesh::IO::Options::VertexNormal;
    mesh.request_vertex_colors();
    mesh.request_vertex_texcoords2D();
    mesh.request_vertex_normals();
	if (!OpenMesh::IO::read_mesh(mesh, s, o, true))
	{
		qDebug() << "Failed to load " << QString::fromStdString(s) << endl;
		exit(1);
	}
    shader = Shader();
    material = Material();
}

void MeshRenderer::Render(MatrixContext* matrixContext)
{
    for (Mesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
    {
        FaceContext faceContext = FaceContext();
        VertexOutContext vertexOutContext[3] = {VertexOutContext(), VertexOutContext(), VertexOutContext()};
        int index = 0, inBoundryCount = 0;
        for (Mesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
        {
            Mesh::Point p = mesh.point(fv_it);
            Mesh::Point vn = mesh.normal(fv_it);
            Mesh::TexCoord2D uv = mesh.texcoord2D(fv_it);
            VertexInContext vertexInContext = VertexInContext();
            vertexInContext.position = glm::vec4(p[0], p[1], p[2], 1);
            vertexInContext.normal = glm::normalize(glm::vec3(vn[0], vn[1], vn[2]));
            vertexInContext.texcoord1 = glm::vec2(uv[0], uv[1]);
            vertexInContext.color = Color::white;
            vertexInContext.vertexIndex = index;

            //������ɫ��
            shader.VertexShading(vertexInContext, vertexOutContext[index], material, matrixContext);

            glm::vec4 pos = vertexOutContext[index].position;

            //�����޳�
            if (-pos.w < pos.x && pos.x < pos.w
                && -pos.w < pos.y && pos.y < pos.w
                && 0 < pos.z && pos.z < pos.w)
            {
                inBoundryCount++;
            }

            //͸�ӳ���
            float w = pos.w;
            pos = pos / w;

            //��դ��
            pos = matrixContext->rasterizationMatrix * pos;

            //ͼԪװ��
            faceContext.screenPosition[index] = glm::ivec2(pos.x + 0.5, pos.y + 0.5);
            faceContext.z[index] = pos.z;
            faceContext.w[index] = pos.w;
            faceContext.vertexIndex[index] = index;
            index++;
        }
        //if (inBoundryCount > 0)
        {
            for (PixelIterator pixelIterator = PixelIterator(faceContext); pixelIterator.CheckValid(); pixelIterator++)
            {
                glm::ivec2 screenPosition = pixelIterator.GetScreenPosition();

                if (0 <= screenPosition.x && screenPosition.x < configuration.resolution.width
                    && 0 <= screenPosition.y && screenPosition.y < configuration.resolution.height)
                {
                    glm::dvec3 barycentricPosition = pixelIterator.GetBarycentricCoordinates();
                    PixelInContext pixelInContext = PixelInContext();
                    PixelOutContext pixelOutContext = PixelOutContext();
                    pixelInContext.screenPosition = screenPosition;
                    
                    pixelInContext.position = vertexOutContext[faceContext.vertexIndex[0]].position * float(barycentricPosition.x) + vertexOutContext[faceContext.vertexIndex[1]].position * float(barycentricPosition.y) + vertexOutContext[faceContext.vertexIndex[2]].position * float(barycentricPosition.z);
                    pixelInContext.normal = glm::normalize(vertexOutContext[faceContext.vertexIndex[0]].normal * float(barycentricPosition.x) + vertexOutContext[faceContext.vertexIndex[1]].normal * float(barycentricPosition.y) + vertexOutContext[faceContext.vertexIndex[2]].normal * float(barycentricPosition.z));
                    pixelInContext.color = vertexOutContext[faceContext.vertexIndex[0]].color * barycentricPosition.x + vertexOutContext[faceContext.vertexIndex[1]].color * barycentricPosition.y + vertexOutContext[faceContext.vertexIndex[2]].color * barycentricPosition.z;
                    pixelInContext.texcoord1 = vertexOutContext[faceContext.vertexIndex[0]].texcoord1 * float(barycentricPosition.x) + vertexOutContext[faceContext.vertexIndex[1]].texcoord1 * float(barycentricPosition.y) + vertexOutContext[faceContext.vertexIndex[2]].texcoord1 * float(barycentricPosition.z);
                    pixelInContext.w = faceContext.w[0] * barycentricPosition.x + faceContext.w[1] * barycentricPosition.y + faceContext.w[2] * barycentricPosition.z;
                    pixelInContext.z = faceContext.z[0] * barycentricPosition.x + faceContext.z[1] * barycentricPosition.y + faceContext.z[2] * barycentricPosition.z;
                    
                    if (pixelInContext.z < configuration.depthBuffer->GetData(screenPosition.x, screenPosition.y))
                    {
                        configuration.depthBuffer->SetData(pixelInContext.z, screenPosition.x, screenPosition.y);

                        //������ɫ��
                        shader.PixelShading(pixelInContext, pixelOutContext, material, matrixContext);

                        configuration.colorBuffer->SetData(pixelOutContext.color, screenPosition.x, screenPosition.y);
                    }
                }
            }
        }
    }
}