
/**
 * Resizes a BMP by a factor of n.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //remember old bitmapfile's header
    long oldW = bi.biWidth;
    long oldH = bi.biHeight;
    DWORD oldSim = bi.biSizeImage;
    DWORD oldSz = bf.bfSize;

    //scale the infile bitmapfile info's for the outfile
    int n = atoi(argv[1]);
    bi.biWidth *= n;
    bi.biHeight *= n;
    int padding_out = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding_out) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(bf) + sizeof(bi);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);


    bi.biWidth = oldW;
    bi.biHeight = oldH;
    bi.biSizeImage = oldSim;
    bf.bfSize = oldSz;
    // determine padding for scanlines
    int padding_in = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        RGBTRIPLE store[bi.biWidth*n];
        int count = 0;
        // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {


                // temporary storage
                RGBTRIPLE triple;
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                for(int p = 0; p < n; p++){
                    store[count] = triple;
                    count++;
                    // write RGB triple to outfile
                    //fwrite(&triple, sizeof(RGBTRIPLE), 1, store);
                }

            }
                // skip over padding, if any
            fseek(inptr, padding_in, SEEK_CUR);

            for(int q = 0; q< n; q++){
                fwrite(store, sizeof(store), 1, outptr);



            // then add it back (to demonstrate how)
            for (int k = 0; k < padding_out; k++)
            {
                fputc(0x00, outptr);
            }
        }

    }
    //for(int q = 0; q < n; q++)
       // fwrite(&store, sizeof(RGBTRIPLE), 1, outptr);

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);


    // success
    return 0;
}

