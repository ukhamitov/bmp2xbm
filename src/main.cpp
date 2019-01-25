//
//  main.cpp
//  bmp2xbm
//
//  Created by Ural Khamitov on 25/01/2019.
//  Copyright © 2019 Ural Khamitov. All rights reserved.
//

#include <iostream>

using namespace std;

int invert = 0;

void parseCommands(const char *options)
{
    if (strcmp(options, "-i") == 0 || strcmp(options, "--invert") == 0)
    {
        printf("Invert");
        invert = 1;
    }
}

void convertBmp(const char *fileName)
{
    FILE *fp = nullptr;
    unsigned char header[54];
    unsigned char *image = nullptr;
    int width = 0, height = 0, temp, black = 1, white = 0, threshold = 10, byteWidth;
    
    if (!(fp = fopen(fileName, "rb")))
        return;
    
    fread(header, sizeof(unsigned char), 54, fp);
    for (int i = 0; i != 4; ++i)
    {
        temp = header[18 + i];
        temp = temp << (8 * i);
        width += temp;
        
        temp = header[22 + i];
        temp = temp << (8 * i);
        height += temp;
    }
    
    image = (unsigned char*)malloc((size_t) width * height * 3);
    if (image == nullptr)
        return;
    
    fread(image, sizeof(unsigned char), (size_t)(long) width * height * 3, fp);
    
    byteWidth = width / 8;
    if (byteWidth * 8 < width)
        byteWidth++;
    
    int img[height][byteWidth * 8];
    memset(img, 0, height * (byteWidth * 8) * sizeof(int));
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (image[((y * width) + x) * 3] <= threshold)
                img[y][x] = invert ? black : white;
            else
                img[y][x] = invert ? white : black;
        }
    }
    
    free(image);
    fclose(fp);
    
    printf("#define image_width %d\n", width);
    printf("#define image_height %d\n", height);
    printf("static char image_bits[] = {\n");
    uint8_t pixval = 0;
    for (int i = 0; i < height; i++)
    {
        printf("  ");
        for (int j = 0; j < byteWidth; j++)
        {
            pixval += (img[height - i - 1][j * 8 + 0] & 1) << 7;
            pixval += (img[height - i - 1][j * 8 + 1] & 1) << 6;
            pixval += (img[height - i - 1][j * 8 + 2] & 1) << 5;
            pixval += (img[height - i - 1][j * 8 + 3] & 1) << 4;
            pixval += (img[height - i - 1][j * 8 + 4] & 1) << 3;
            pixval += (img[height - i - 1][j * 8 + 5] & 1) << 2;
            pixval += (img[height - i - 1][j * 8 + 6] & 1) << 1;
            pixval += (img[height - i - 1][j * 8 + 7] & 1) << 0;
            printf("0x%02X, ", pixval);
            pixval = 0;
        }
        printf("\n");
    }
    printf("}\n");
}

int main(int argc, const char* argv[])
{
    if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)))
    {
        printf("Usage: bmp2xbm [file.bmp] [options]\n");
        printf("  -h, --help    Show command line usage\n");
        printf("  -i, --invert  Invert image colors\n");
        return 0;
    }
    if (argc == 3)
        parseCommands(argv[2]);
    convertBmp(argv[1]);
    return 0;
}
