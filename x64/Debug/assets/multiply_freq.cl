// OpenCL usa float2 para representar números complejos (real, imaginario)
float2 complex_mul(float2 a, float2 b) {
    return (float2)(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

__kernel void multiply_freq(__global const float2* a,
    __global const float2* b,
    __global float2* result)
{
    int idx = get_global_id(0);
    result[idx] = complex_mul(a[idx], b[idx]);
}