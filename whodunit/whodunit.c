// Copies a BMP file and manipulates the pixels from the copy

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // Ensures proper usage
    if (argc != 3)
    {
        printf("Usage: copy infile outfile\n");
        return 1;
    }

    // Remembers filenames
    char *infile = argv[1];
    char *outfile = argv[2];

    // Opens input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // Opens output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 3;
    }

    // Reads infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Reads infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // Ensures infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        printf("Unsupported file format.\n");
        return 4;
    }

    // Writes outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Writes outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Determines padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterates over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // Iterates over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // Temporary storage
            RGBTRIPLE triple;

            // Reads RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
           
	    // Manipulates the RGB pixels
            if (triple.rgbtRed <= 0xff && triple.rgbtRed >= 0xfa 
                && triple.rgbtBlue <= 0xff && triple.rgbtBlue >= 0x00
                && triple.rgbtGreen <= 0xff && triple.rgbtGreen >= 0x00)
            {
                triple.rgbtRed = 0x00;
                
                triple.rgbtGreen = 0x00;
                
                triple.rgbtBlue = 0x00;
            }
            else
            {
                triple.rgbtRed = 0xff;
                
                triple.rgbtGreen = 0xff;
                
                triple.rgbtBlue = 0xff;
            }
            
            // Writes RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // Skips over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // Then adds it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // Closes infile
    fclose(inptr);

    // Closes outfile
    fclose(outptr);

    // Success
    return 0;
}
