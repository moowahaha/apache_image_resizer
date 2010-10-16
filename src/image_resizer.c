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

imageType determineImageType(char *requestedFile) {
    int availableImageTypes = sizeof(Images) / sizeof(imageType);
    int i = 0;

    char imageExtension[5];
    bzero(&imageExtension, 5);

    char *fileChar = strrchr(requestedFile, '.');
    fileChar++;

    strcpy(&imageExtension, fileChar);

    imageType selectedImageType;

    for(i = 0; i < availableImageTypes; i++) {
        if (strcmp(imageExtension, Images[i].extension) == 0) {
            selectedImageType = Images[i];
            break;
        }
    }

    return (selectedImageType);
}

int hexify(char *hexString, int offset) {
    char colorString[2];
    strncpy(&colorString, hexString + offset, 2);
    return strtoul(colorString, 0, 16);
}

int offset(int calculatedLength, int requestedLength) {
    int offset = 0;

    if (calculatedLength > requestedLength) {
        offset = (int)((calculatedLength - requestedLength) / 2);
    }
    else {
        offset = (int)((requestedLength - calculatedLength) / 2);
    }

    return (offset);
}

int integerFromQuery(char *param) {
    char queryValue[10];
    cgiFormStringNoNewlines(param, queryValue, sizeof(queryValue));
    return (atoi(queryValue));
}

void resize (gdImagePtr original, gdImagePtr destination) {
    float originalRatio = (float)original->sx / original->sy;
    float destinationRatio = (float)destination->sx / destination->sy;

    int destinationX = destination->sx;
    int destinationY = destination->sy;

    if (destinationRatio > originalRatio) {
        destinationX = floor(destination->sy * originalRatio);
    }
    else {
        destinationY = floor(destination->sx / originalRatio);

    }

    gdImageCopyResampled(
        destination,
        original,
        offset(destinationX, destination->sx),
        offset(destinationY, destination->sy),
        0,
        0,
        destinationX,
        destinationY,
        original->sx,
        original->sy
    );
}

void setBackgroundColor(gdImagePtr destination, char *paddingColor) {
    gdImageFill(
        destination,
        0,
        0,
        gdImageColorAllocate(
            destination,
            hexify(paddingColor, 0),
            hexify(paddingColor, 2),
            hexify(paddingColor, 4)
        )
    );
}

int cgiMain() {
    FILE *imageFh = openImage(getenv("IMAGE_ROOT"), cgiPathInfo);
    imageType image = determineImageType(cgiPathInfo);

    cgiHeaderContentType(image.mimeType);

    gdImagePtr original = (*image.instantiateImage)(imageFh);
    gdImagePtr destination = gdImageCreateTrueColor(integerFromQuery("width"), integerFromQuery("height"));

    setBackgroundColor(destination, getenv("PADDING_COLOR"));
    resize(original, destination);

    (*image.output)(destination, cgiOut);

    gdImageDestroy(original);
    gdImageDestroy(destination);

    return 0;
}
