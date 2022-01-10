namespace glutUtil
{
    int glhUnProjectVecf(float winx, float winy, float winz, float* modelview, float* projection, int* viewport, float* objectCoordinate);

    void MultiplyMatrixByVector4by4OpenGL_FLOAT(float* resultvector, const float* matrix, const float* pvector);

    void MultiplyMatrices4by4OpenGL_FLOAT(float* result, float* matrix1, float* matrix2);

    //This code comes directly from GLU except that it is for float
    int glhInvertMatrixf2(float* m, float* out);

}