#include "cgic.h"
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct {
    char mimeType[11];
    char extension[4];
    gdImagePtr (*instantiateImage)(FILE *);
    void (*output)(gdImagePtr, FILE *);
} imageType;

imageType Images[] = {
    {"image/jpeg", "jpg", gdImageCreateFromJpeg, gdImageJpeg},
    {"image/jpeg", "jpeg", gdImageCreateFromJpeg, gdImageJpeg},
    {"image/gif", "gif", gdImageCreateFromGif, gdImageGif},
    {"image/png", "png", gdImageCreateFromPng, gdImagePng}
};

FILE *openImage(char *imageRoot, char *requestedFile) {
    FILE *fh;
    char *fullImagePath = malloc(strlen(imageRoot) + strlen(requestedFile) + 1);

    strncpy(fullImagePath, imageRoot, strlen(imageRoot));
    strncat(fullImagePath, "/", 1);
    strncat(fullImagePath, requestedFile, strlen(requestedFile));

    fh = fopen(fullImagePath, "rb");

    free(fullImagePath);

    return fh;
}

imageType determineImage(char *requestedFile) {
    int availableImageTypes = sizeof(Images) / sizeof(imageType);
    int i = 0;

    char imageExtension[4];
    bzero(&imageExtension, 5);

    char *fileChar = strrchr(requestedFile, '.');
    fileChar++;

    while(*fileChar != '\0') {
        imageExtension[i] = *fileChar;
        i++;
        fileChar++;
    }

    imageType selectedImageType;

    for(i = 0; i < availableImageTypes; i++) {
        if (strcmp(imageExtension, Images[i].extension) == 0) {
            selectedImageType = Images[i];
            break;
        }
    }

    return (selectedImageType);
}

int cgiMain() {
    FILE *imageFh = openImage(getenv("IMAGE_ROOT"), cgiPathInfo);
    imageType image = determineImage(cgiPathInfo);
    cgiHeaderContentType(image.mimeType);

    gdImagePtr original = (*image.instantiateImage)(imageFh);
    (*image.output)(original, cgiOut);

    gdImageDestroy(original);


//    in = fopen(, "rb");
//  fprintf(cgiOut, "<HTML><HEAD>\n");
//  fprintf(cgiOut, "<TITLE>cgic test</TITLE></HEAD>\n");
//  fprintf(cgiOut, "<BODY><H1>cgic test</H1>\n");
//  fprintf(cgiOut, "path: %s<br/>", cgiPathInfo);

//  char *blah;
//  blah = getenv("TERRY");
//  fprintf(cgiOut, "env: %s<br/>", blah);

//  char queryParam[1024];
//  cgiFormStringNoNewlines("a", queryParam, sizeof(queryParam));
//  fprintf(cgiOut, "param: %s<br/>", queryParam);
//  fprintf(cgiOut, "</BODY></HTML>\n");
  return 0;
}
