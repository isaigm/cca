__kernel void perform_step(__global float *cells, __global float *tempCells, __global unsigned char *pixels,
 __global float *mat, __global float *color,
 int rows, int cols)
{
    int row = get_global_id(0);
    int col = get_global_id(1);
    if(row >= rows || col >= cols) return;
    int mrow   = 0;
    int mcol   = 0;
    float wsum = 0;
    for(int i = row - 1; i <= row + 1; i++, mrow++)
    {
        for(int j = col - 1, mcol = 0; j <= col + 1; j++, mcol++)
        {
            int ti = i % rows;
            int tj = j % cols;
            wsum += mat[mcol + mrow * 3] * cells[tj + ti * cols];
        }
    }
    wsum = activation(wsum);
    wsum = clamp(wsum, 0.0f, 1.0f);
    tempCells[col + row * cols] = wsum;
    unsigned char val = (unsigned char) 255 * wsum;
    pixels[(col + row * cols) * 4]     = val * color[0];
    pixels[(col + row * cols) * 4 + 1] = val * color[1];
    pixels[(col + row * cols) * 4 + 2] = val * color[2];
    pixels[(col + row * cols) * 4 + 3] = val;
<<<<<<< HEAD
}
=======
}
>>>>>>> 8794707de726ab18fb05fa759d78b77d12d1bfb2
