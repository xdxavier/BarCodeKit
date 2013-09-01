//
//  BCKCode128Code.m
//  BarCodeKit
//
//  Created by Jaanus Siim on 8/25/13.
//  Copyright (c) 2013 Oliver Drobnik. All rights reserved.
//

#import "BCKCode128Code.h"
#import "BCKCode128ContentCodeCharacter.h"
#import "NSString+BCKCode128Helpers.h"

@implementation BCKCode128Code {
    BCKCode128Version _barcodeVersion;
}

- (BCKCode *)initWithContent:(NSString *)content
{
    self = [super initWithContent:content];

    if (self)
    {
        BCKCode128Version versionUsed = [BCKCode128ContentCodeCharacter code128VersionNeeded:content];
        if (versionUsed == Code128Unsupported)
        {
            NSLog(@"String '%@' cannot be encoded in Code128", content);
            return nil;
        }

        _barcodeVersion = versionUsed;
        _content = [content copy];
    }

    return self;
}

#pragma mark - Subclass Methods

- (NSArray *)codeCharacters
{
    NSMutableArray *tmpArray = [NSMutableArray array];

    BCKCode128Version writeVersion = _barcodeVersion;

    // start marker
    BCKCode128CodeCharacter *startCode = [BCKCode128CodeCharacter startCodeForVersion:writeVersion];
    [tmpArray addObject:startCode];

    // check counter
    NSUInteger check = [startCode position];

    NSMutableString *content = [_content mutableCopy];

    NSUInteger barcodeIndex = 0;
    NSUInteger contentIndex = 0;

    while ([content length] > 0)
    {
        BCKCode128Version continueWithVersion = [self _versionToContinue:writeVersion contentIndex:contentIndex remainingContent:content];

        if (continueWithVersion != writeVersion)
        {
            BCKCode128CodeCharacter *switchCode = [BCKCode128CodeCharacter switchCodeToVersion:continueWithVersion];

            check += ([switchCode position] * (barcodeIndex + 1));
            [tmpArray addObject:switchCode];
            barcodeIndex++;

            writeVersion = continueWithVersion;
        }

        NSString *toEncode = [self _nextCharacterToEncode:content writeVersion:writeVersion];
        contentIndex += [toEncode length];

        BCKCode128ContentCodeCharacter *codeCharacter = [BCKCode128ContentCodeCharacter codeCharacterForCharacter:toEncode codeVersion:writeVersion];

        // (character position in Code 128 table) x (character position in string 'starting from 1')
        check += ([codeCharacter position] * (barcodeIndex + 1));
        [tmpArray addObject:codeCharacter];

        barcodeIndex++;
        [content deleteCharactersInRange:NSMakeRange(0, [toEncode length])];
    }

    // find remainder with magic number 103
    NSUInteger remainder = check % 103;

    // check character at found position
    [tmpArray addObject:[BCKCode128CodeCharacter characterAtPosition:remainder]];


    // end marker
    [tmpArray addObject:[BCKCode128CodeCharacter stopCharacter]];

    return [tmpArray copy];
}

- (NSUInteger)horizontalQuietZoneWidth
{
    return 10;
}

- (CGFloat)aspectRatio
{
    return 0;  // do not use aspect
}

- (CGFloat)fixedHeight
{
    return 30;
}

- (CGFloat)_captionFontSizeWithOptions:(NSDictionary *)options
{
    return 10;
}

- (BOOL)markerBarsCanOverlapBottomCaption
{
    return NO;
}

- (NSString *)captionTextForZone:(BCKCodeDrawingCaption)captionZone
{
    if (captionZone == BCKCodeDrawingCaptionTextZone)
    {
        return _content;
    }

    return nil;
}

- (UIFont *)_captionFontWithSize:(CGFloat)fontSize
{
    UIFont *font = [UIFont boldSystemFontOfSize:fontSize];

    return font;
}

- (BOOL)allowsFillingOfEmptyQuietZones
{
    return NO;
}

- (NSString *)_nextCharacterToEncode:(NSString *)content writeVersion:(BCKCode128Version)writeVersion
{
    if (writeVersion == Code128C)
    {
        return [content substringWithRange:NSMakeRange(0, 2)];
    }

    return [content substringWithRange:NSMakeRange(0, 1)];
}

// Try to perform barcode length optimizations described at http://en.wikipedia.org/wiki/Code_128#Barcode_Length_Optimization_Using_Code_Set_C
- (BCKCode128Version)_versionToContinue:(BCKCode128Version)currentWriteVersion contentIndex:(NSUInteger)index remainingContent:(NSMutableString *)remainingContent
{
    if (currentWriteVersion != Code128C)
    {
        if ([remainingContent length] > 4 && [remainingContent containsOnlyNumbers])
        {
            return Code128C;
        }
    }

    if (currentWriteVersion != Code128C)
    {
        return currentWriteVersion;
    }

    if ([remainingContent length] < 2)
    {
        return Code128A;
    }

    return currentWriteVersion;
}

@end
