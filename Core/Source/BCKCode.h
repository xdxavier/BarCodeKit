//
//  BCKCode.h
//  BarCodeKit
//
//  Created by Oliver Drobnik on 8/9/13.
//  Copyright (c) 2013 Oliver Drobnik. All rights reserved.
//

// Drawing options string constants
extern NSString * const BCKCodeDrawingBarScaleOption;
extern NSString * const BCKCodeDrawingPrintCaptionOption;
extern NSString * const BCKCodeDrawingCaptionFontNameOption;
extern NSString * const BCKCodeDrawingMarkerBarsOverlapCaptionPercentOption;
extern NSString * const BCKCodeDrawingFillEmptyQuietZonesOption;
extern NSString * const BCKCodeDrawingDebugOption;
extern NSString * const BCKCodeDrawingShowCheckDigitsOption;

/**
 Caption Zones
 */
typedef NS_ENUM(NSUInteger, BCKCodeDrawingCaption)
{
	/**
	 The quiet zone to the left of the left start marker
	 */
	BCKCodeDrawingCaptionLeftQuietZone,
	
	/**
	 The digit zone between the left start marker and the middle marker
	 */
	BCKCodeDrawingCaptionLeftNumberZone,
	
	/**
	 The digit zone between the middle marker and the right end marker
	 */
	BCKCodeDrawingCaptionRightNumberZone,
	
	/**
	 The quiet zone to the right of the right end marker
	 */
	BCKCodeDrawingCaptionRightQuietZone,
	
	/**
	 The text zone between the left and right start and end markers
	 */
	BCKCodeDrawingCaptionTextZone
};

/**
 This is the protocol all BCKCode subclasses must conform to.
*/
@protocol BCKCoding <NSObject>

@required

/**
 The array of individual code characters the barcode comprises. When subclassing BCKCode this array of BCKCodeCharacter instances make up the bit string representation of the barcode content string. Defaults to `nil`.
 */
@property (nonatomic, readonly) NSArray *codeCharacters;

/**
 The barcode class' standard. For example "International standard ISO/IEC 15420". Defaults to `nil`.
 */
+ (NSString *)barcodeStandard;

/**
 Human readable description of the barcode class. For example "EAN-8". Defaults to `nil`.
 */
+ (NSString *)barcodeDescription;

/**
 Checks if the content string is encodable by the BCKCode subclass. Defaults to `NO`.
 @param content The content string to be encoded by the BCKCode subclass. Any check digits will be generated by the subclass and should not be included in the content string.
 @param error Optional output parameter providing an `NSError` in case the content string cannot be encoded by the BCKCode subclass.
 @return `YES` if the content string is encodable, in which case the error object is set to nil. `NO` if the contents can not be encoded, in which case the error object contains error information.
 */
+ (BOOL)canEncodeContent:(NSString *)content error:(NSError **)error;

/**
 The caption text to display in the given caption zone, or `nil` for no caption text. Defaults to `nil`. Subclasses can return the check digit or other text and also alter the caption text by providing rendering options.
 @param captionZone The BCKCodeDrawingCaption zone specifying the required text zone.
 @param options The rendering options.
 @return The caption text to display in this zone, or `nil` for no caption text.
 */
- (NSString *)captionTextForZone:(BCKCodeDrawingCaption)captionZone withRenderOptions:(NSDictionary *)options;

@optional

/**
 @name Creating Barcodes
 */

/**
 Root initializer for subclasses of the BCKCode class cluster. You should not call this on BCKCode directly. Always use a concrete subclass depending on the kind of barcode you want to generate. The initializer copies the content string to the _content ivar. Subclasses only need to override this initializer if they add additional properties. This initialiser calls the subclass' canEncodeContent method to determine whether the content string can be encoded.
 @param content The content string to be encoded by the subclass.
 @param error Optional output parameter to take an `NSError` in case the content cannot be encoded by this barcode class. Pass `nil` if not error information is not required.
 @return The requested BCKCode subclass. Returns `nil` if the provided content cannot be encoded by the requested BCKCode subclass, the error object will then contain error details.
 */
- (instancetype)initWithContent:(NSString *)content error:(NSError**)error;

/**
 @name Configuring barcodes
 */

/**
 The width of the horizontal quiet zone (in bar units) on the left and right sides of the barcode. Defaults to 0 to indicate there are no horizontal quiet zones.
 */
- (NSUInteger)horizontalQuietZoneWidth;

/**
 The aspect ratio between the length and width of the rendered barcode. Defaults to 1.
 */
- (CGFloat)aspectRatio;

/**
 The fixed height of the rendered barcode. Defaults to 0 to indicate there is no fixed height.
 */
- (CGFloat)fixedHeight;

/**
 Whether the subclass allows for marker bars to reach into the bottom caption region. If `YES`, then the percentage of overlap can be specified with the BCKCodeDrawingMarkerBarsOverlapCaptionPercentOption. Defaults to `NO`.
 */
- (BOOL)markerBarsCanOverlapBottomCaption;

/**
 Whether the barcode allows for quite zones to be filled with angle brackets. If `YES`, then quiet zones are filled if the BCKCodeDrawingFillEmptyQuietZonesOption is specified. Defaults to `NO`.
 */
- (BOOL)allowsFillingOfEmptyQuietZones;

/**
 The font used for text captions if no other font is specified via BCKCodeDrawingCaptionFontNameOption. Defaults to Helvetica.
 */
- (NSString *)defaultCaptionFontName;

/**
 Whether the barcode includes check digits in the caption printed below the barcode. By default any check digits that are not markers are included in the caption text. Subclasses can indicate support for this excluding check digits from the caption by overriding this method and returning `YES`. Defaults to `NO`.
 */
- (BOOL)showCheckDigitsInCaption;

/**
 Whether the barcode supports showing caption text below the barcode. Subclasses can indicate they never require a caption by overriding this method and returning `NO`. Defaults to `YES`.
 */
- (BOOL)requiresCaptionText;

/**
 @name Deprecated Methods
 */

/**
 Root initializer for sub-classes of the BCKCode class cluster. You should not call this on BCKCode directly, but always on concrete subclasses based on the kind of code you want to generate.
 @warning This method is deprecated, use - [BCKCode initWithContent:error:] instead.
 @param content The content string for the barcode.
 @return The requested BCKCode subclass. Returns `nil` if the content string cannot be encoded using the requested BCKCode subclass.
 */
- (instancetype)initWithContent:(NSString *)content __attribute__((deprecated("use - [BCKCode initWithContent:error:] instead")));

@end

/**
 This is the base class for all barcode variants:
 
 - BCKEAN8Code - EAN-8 - International Standard ISO/IEC 15420
 - BCKEAN13Code - EAN-13 (13-digit EAN or 12-digit UPC-A) - International Standard ISO/IEC 15420
 - BCKUPCECode - UPC-E (shortened UPC) - International Standard ISO/IEC 15420
 - BCKCode39Code - Code 39 - international standard ISO/IEC 16388
 - BCKCode93Code - Code 93 - no international standard
 - BCKInterleaved2of5Code - Interleaved 2 of 5 - International standard ISO/IEC 16390
 - BCKCode128Code - Code 128 - International Standard ISO/IEC 15417
 - BCKCode11Code - Code 11 - no international standard
 - BCKMSICode - MSI or Modified Plessey barcode - no international standard
 - BCKPharmacodeOneTrack - Pharmacode One Track - no international standard
 - BCKCodabarCode - Codabar - no international standard
 - BCKStandard2of5Code - Codabar - no international standard
 - BCKFacingIdentificationMarkCode - FIM - no international standard
 - BCKEAN2SupplementCode - no international standard
 - BCKEAN5SupplementCode - no international standard
 
 For rendering codes several options can be combined in an options dictionary:
 
 - **BCKCodeDrawingBarScaleOption** - Multiplier for the bar width (default: 1)
 - **BCKCodeDrawingPrintCaptionOption** - Whether the code caption should be printed (default: `NO`)
 - **BCKCodeDrawingCaptionFontNameOption** - Which font face name to use for the caption (default: 'OCRB' for EAN/UPC and 'Helvetica' otherwise)
 - **BCKCodeDrawingMarkerBarsOverlapCaptionPercentOption** - Percentage of the caption height covered by elongated marker bars (default: 1.0)
 - **BCKCodeDrawingFillEmptyQuietZonesOption** - Whether quiet zones should be filled with angle brackets (default: `NO`)
 - **BCKCodeDrawingDebugOption** - Whether caption areas should be tinted for debugging (default: `NO`)
 - **BCKCodeDrawingShowCheckDigitsOption** - Whether check digits are to be included in the printed caption (default: `NO`)
 */
@interface BCKCode : NSObject <BCKCoding>
{
	NSString *_content;
	NSArray *_codeCharacters;       // This ivar is declared as a public ivar to enable subclass access
}

/**
 @name Drawing Barcodes
 */

/**
 Renders the barcode symbol into a graphics context.
 @param context The graphics context to render into.
 @param options The rendering options.
 */
- (void)renderInContext:(CGContextRef)context options:(NSDictionary *)options;

/**
 Calculates the graphics context's size using the provided rendering options.
 @param options An NSDictionary containig the requested rendering options.
 @returns The size required to fit the barcode's rendered barcode symbol.
 */
- (CGSize)sizeWithRenderOptions:(NSDictionary *)options;

/**
 @name Getting Information about Barcodes
 */

/**
 The content string for the receiver that will be converted into the barcode.
 */
@property (nonatomic, readonly) NSString *content;

@end
