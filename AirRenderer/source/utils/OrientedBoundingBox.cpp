#include <include/utils/OrientedBoundingBox.h>
glm::mat3 OrientedBoundingBox::computeCovarianceMatrix(glm::vec3* pVertices, int numVertices)
{
    glm::mat3 covariance = glm::mat3();
    std::vector<glm::vec3> pVectors = std::vector<glm::vec3>(numVertices);

    //Compute the average x,y,z  
    glm::vec3 avg = glm::vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < numVertices; i++) {
        pVectors[i] = pVertices[i];
        avg = avg + pVertices[i];
    }
    avg = avg / float(numVertices);
    for (int i = 0; i < numVertices; i++)
        pVectors[i] -= avg;

    //Compute the covariance   
    for (int c = 0; c < 3; c++)
    {
        float data[3] = { 0, 0, 0 };
        for (int r = c; r < 3; r++)
        {
            covariance[c][r] = 0.0;
            float acc = 0.0f;
            //cov(X,Y)=E[(X-x)(Y-y)]  
            for (int i = 0; i < numVertices; i++)
            {
                data[0] = pVectors[i].x;
                data[1] = pVectors[i].y;
                data[2] = pVectors[i].z;
                acc += data[c] * data[r];
            }

            acc /= numVertices;
            covariance[c][r] = acc;
            //symmetric  
            covariance[r][c] = acc;
        }
    }
    return covariance;
}
void OrientedBoundingBox::jacobiSolver(glm::mat3& matrix, float* eValue, glm::vec3* eVectors) {
    float eps1 = (float)0.00001;
    float eps2 = (float)0.00001;
    float eps3 = (float)0.00001;

    //qDebug() << "----------the 1.e-5----------";
    //qDebug() << eps1;

    float p, q, spq;
    float cosa = 0, sina = 0;  //cos(alpha) and sin(alpha)  
    float temp;
    float s1 = 0.0f;    //sums of squares of diagonal  
    float s2;          //elements  

    bool flag = true;  //determine whether to iterate again  
    int iteration = 0;   //iteration counter  

    glm::vec3 mik = glm::vec3();//used for temporaty storage of m[i][k]  
    float data[3] = { 0, 0, 0 };
    glm::mat3 t = glm::mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);//To store the product of the rotation matrices.  
    do {

        iteration++;

        for (int i = 0; i < 2; i++)
        {
            for (int j = i + 1; j < 3; j++)
            {
                if (std::abs(matrix[j][i]) < eps1)
                    matrix[j][i] = 0.0;
                else {
                    q = std::abs(matrix[i][i] - matrix[j][j]);
                    if (q > eps2) {
                        p = (2.0f * matrix[j][i] * q) / (matrix[i][i] - matrix[j][j]);
                        spq = (float)std::sqrt(p * p + q * q);
                        cosa = (float)std::sqrt((1.0f + q / spq) / 2.0f);
                        sina = p / (2.0f * cosa * spq);
                    }
                    else
                        sina = cosa = 0.7071067811865;

                    for (int k = 0; k < 3; k++) {
                        temp = t[i][k];
                        t[i][k] = temp * cosa + t[j][k] * sina;
                        t[j][k] = temp * sina - t[j][k] * cosa;
                    }
                    for (int k = i; k < 3; k++)
                    {
                        if (k > j) {
                            temp = matrix[k][i];
                            matrix[k][i] = cosa * temp + sina * matrix[k][j];
                            matrix[k][j] = sina * temp - cosa * matrix[k][j];
                        }
                        else
                        {
                            data[k] = matrix[k][i];
                            matrix[k][i] = cosa * data[k] + sina * matrix[j][k];

                            if (k == j)
                                matrix[k][j] = sina * data[k] - cosa * matrix[k][j];
                        }
                    }
                    data[j] = sina * data[i] - cosa * data[j];

                    for (int k = 0; k <= j; k++)
                    {
                        if (k <= i)
                        {
                            temp = matrix[i][k];
                            matrix[i][k] = cosa * temp + sina * matrix[j][k];
                            matrix[j][k] = sina * temp - cosa * matrix[j][k];

                        }
                        else
                            matrix[j][k] = sina * data[k] - cosa * matrix[j][k];

                    }
                }
            }
        }
        s2 = 0.0f;
        for (int i = 0; i < 3; i++)
        {
            eValue[i] = matrix[i][i];
            s2 += eValue[i] * eValue[i];
        }

        if (std::abs(s2) < (float)1.e-5 || std::abs(1.0 - s1 / s2) < eps3)
            flag = false;
        else
            s1 = s2;
    } while (flag);

    eVectors[0].x = t[0][0];
    eVectors[0].y = t[0][1];
    eVectors[0].z = t[0][2];
    eVectors[1].x = t[1][0];
    eVectors[1].y = t[1][1];
    eVectors[1].z = t[1][2];
    eVectors[2].x = t[2][0];
    eVectors[2].y = t[2][1];
    eVectors[2].z = t[2][2];

    //qDebug() << "----------------eVector without transform-----------";
    //for (int i = 0; i < 3; i++)
    //    printVector(eVectors[i]);
    //System.out.println();

    mik.x = data[0];
    mik.y = data[1];
    mik.z = data[2];
    glm::vec3 cross = glm::cross(eVectors[0], eVectors[1]);
    if ((glm::dot(cross, eVectors[2]) < 0.0f))
        eVectors[2] = glm::vec3(1.0 / eVectors[2].x, 1.0 / eVectors[2].y, 1.0 / eVectors[2].z);

}
void OrientedBoundingBox::schmidtOrthogonal(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) {
    glm::normalize(v0);
    v1 = v1 - glm::dot(v0, v1) * v0;
    glm::normalize(v1);
    v2 = glm::normalize(glm::cross(v0, v1));
}