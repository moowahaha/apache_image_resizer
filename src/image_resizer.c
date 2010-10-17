#include "cgic.h"
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

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

char *ExpectedEnvironment[] = {"IMAGE_ROOT", "PADDING_COLOR", "MAX_HEIGHT", "MAX_WIDTH", NULL};

void throwError(int httpCode, char *message) {
    cgiHeaderStatus(httpCode, message);
    fprintf(stderr, "%s\n", message);
    fprintf(cgiOut, "Error: %s\n", message);
    free(message);
    exit(255);
}

void checkEnvironment() {
    char **env = ExpectedEnvironment;
    int missingEnvironment = 0;
    
    while(*env) {
        if (!getenv(*env)) {
            if (!missingEnvironment) {
                fprintf(stderr, "Missing environment settings:");
            }
            
            fprintf(stderr, " ");
            fprintf(stderr, *env);
            missingEnvironment = 1;
        }
        env++;
    }

    if (missingEnvironment) {
        fprintf(stderr, "\n");
        cgiHeaderStatus(500, "Not setup right!");
        exit(255);
    }
}

void checkRequestedDimensions(int requestedWidth, int requestedHeight, char *file) {
    int allowableWidth = atoi(getenv("MAX_WIDTH"));
    int allowableHeight = atoi(getenv("MAX_HEIGHT"));

    if (!requestedWidth || !requestedHeight) {
        char *message = malloc(33);
        strcat(message, "Missing parameters:");

        if (!requestedWidth) {
            strcat(message, " width");
        }

        if (!requestedHeight) {
            strcat(message, " height");
        }
        
        throwError(400, message);
    }

    if (requestedWidth > allowableWidth || requestedHeight > allowableHeight) {
        char *message = malloc(512);
        sprintf(message, "Requested dimension exceeds maximum allowable size: %s (requested %dx%d)", file, requestedWidth, requestedHeight);
        throwError(413, message);
    }
}

void checkOpenErrors(FILE *fh, char *file) {
    if (fh) {
        return;
    }

    char *message = malloc(strlen(strerror(errno)) + strlen(file) + 3);
    sprintf(message, "%s: %s", strerror( errno ), file);

    if (errno == ENOENT) {
        throwError(404, message);
    }
    else if (errno == EACCES) {
        throwError(403, message);
    }
    else {
        throwError(500, message);
    }
}

FILE *openImage(char *imageRoot, char *requestedFile) {
    if (!strlen(requestedFile)) {
        char actualMessage[] = "No image defined.";
        char *message = malloc(strlen(actualMessage) + 1);
        strcat(message, actualMessage);
        throwError(400, message);
    }

    FILE *fh;
    char *fullImagePath = malloc(strlen(imageRoot) + strlen(requestedFile) + 1);

    if (strstr(requestedFile, "..")) {
        char *message = malloc(24 + strlen(requestedFile));
        sprintf(message, "Not allowed: %s", requestedFile);
        throwError(403, message);
    }

    strncpy(fullImagePath, imageRoot, strlen(imageRoot));
    strncat(fullImagePath, "/", 1);
    strncat(fullImagePath, requestedFile, strlen(requestedFile));

    fh = fopen(fullImagePath, "rb");

    free(fullImagePath);

    checkOpenErrors(fh, requestedFile);

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
    checkEnvironment();

    int requestedWidth = integerFromQuery("width");
    int requestedHeight = integerFromQuery("height");

    checkRequestedDimensions(requestedWidth, requestedHeight, cgiPathInfo);

    FILE *imageFh = openImage(getenv("IMAGE_ROOT"), cgiPathInfo);
    imageType image = determineImageType(cgiPathInfo);

    cgiHeaderContentType(image.mimeType);

    gdImagePtr original = (*image.instantiateImage)(imageFh);
    gdImagePtr destination = gdImageCreateTrueColor(requestedWidth, requestedHeight);

    setBackgroundColor(destination, getenv("PADDING_COLOR"));
    resize(original, destination);

    (*image.output)(destination, cgiOut);

    gdImageDestroy(original);
    gdImageDestroy(destination);

    return 0;
}
