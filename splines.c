double catmullRomSpline(
    f4 x, 
    f4 v0,f4 v1,
    f4 v2,f4 v3)
{

    f4 c1,c2,c3,c4;

    c1 =          M12*v1;
    c2 = M21*v0          + M23*v2;
    c3 = M31*v0 + M32*v1 + M33*v2 + M34*v3;
    c4 = M41*v0 + M42*v1 + M43*v2 + M44*v3;

    return(((c4*x + c3)*x +c2)*x + c1);
}

static signed char coefs[16] = {
    -1, 2,-1, 0,
     3,-5, 0, 2,
    -3, 4, 1, 0,
     1,-1, 0, 0 };

void spline( const float *key, int num, int dim, float t, float *v )
{
    const int size = dim + 1;

    // find key
    int k = 0;
    while( key[k*size] < t )
        k++;

    // interpolant
    const float h = (t-key[(k-1)*size])/(key[k*size]-key[(k-1)*size]);

    // init result
    for( int i=0; i < dim; i++ ) v[i] = 0.0f;

    // add basis functions
    for( int i=0; i<4; i++ )
    {
        int kn = k+i-2; if( kn<0 ) kn=0; else if( kn>(num-1) ) kn=num-1;

        const signed char *co = coefs + 4*i;

        const float b  = 0.5f*(((co[0]*h + co[1])*h + co[2])*h + co[3]);

        for( int j=0; j < dim; j++ ) v[j] += b * key[kn*size+j+1];
    }
}