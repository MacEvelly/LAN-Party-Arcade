# Header.bmp Logo Guide

## Overview
The `Header.bmp` file displays a logo at the top of the connection screen on the ESP32-2432S028 display.

## Specifications
- **Resolution**: 200x64 pixels
- **Format**: 24-bit BMP (no compression)
- **Color Space**: RGB
- **File Size**: ~38KB (200 × 64 × 3 bytes + header)

## Creating Your Logo

### Method 1: Using GIMP (Free)
1. Create new image: 200x64 pixels
2. Design your logo (text, graphics, etc.)
3. Export as BMP:
   - File → Export As → `Header.bmp`
   - Compatibility Options: **Do not write color space information**
   - Advanced Options: **24 bits R8 G8 B8**
   - DO NOT check "Run-Length Encoded"

### Method 2: Using Photoshop
1. New document: 200x64 pixels
2. Design your logo
3. File → Save As → BMP
4. File Format: **Windows**
5. Depth: **24 bit**
6. DO NOT use compression

### Method 3: Using Paint.NET (Free, Windows)
1. New image: 200x64 pixels
2. Design your logo
3. File → Save As → `Header.bmp`
4. Bit Depth: **24-bit**

### Method 4: Using ImageMagick (Command Line)
```bash
# Convert from PNG/JPG to BMP
convert input.png -resize 200x64! -type TrueColor -depth 8 Header.bmp

# Verify the format
identify Header.bmp
# Should output: Header.bmp BMP 200x64 200x64+0+0 8-bit sRGB
```

### Method 5: Using Python + Pillow
```python
from PIL import Image, ImageDraw, ImageFont

# Create new image
img = Image.new('RGB', (200, 64), color='black')
draw = ImageDraw.Draw(img)

# Add text (adjust font size as needed)
try:
    font = ImageFont.truetype("arial.ttf", 24)
except:
    font = ImageFont.load_default()

text = "LAN PARTY"
bbox = draw.textbbox((0, 0), text, font=font)
text_width = bbox[2] - bbox[0]
text_height = bbox[3] - bbox[1]
position = ((200 - text_width) // 2, (64 - text_height) // 2)
draw.text(position, text, fill='white', font=font)

# Save as BMP
img.save('Header.bmp', 'BMP')
```

## Design Tips
1. **Keep it simple**: The display is only 200px wide - avoid tiny details
2. **High contrast**: Use light text on dark background (or vice versa)
3. **Center alignment**: Logo appears at top-center of screen
4. **Test on device**: Colors may look different on TFT vs computer screen
5. **Safe area**: Leave 5-10px margins on all sides

## Color Considerations
- Display supports RGB565 (65,536 colors) but BMP should be 24-bit RGB888
- ESP32 automatically converts during rendering
- Avoid gradients (may show banding on TFT)
- Pure colors work best: White (#FFFFFF), Black (#000000), Red (#FF0000), Blue (#0000FF), etc.

## Example Logos
- **Text only**: "LAN PARTY ARCADE" in bold font
- **Icon + text**: Gamepad icon + "ARCADE"
- **Retro style**: Pixel art logo
- **Minimal**: Just initials "LPA"

## Fallback Behavior
If `Header.bmp` is missing or invalid:
- ESP32 will skip logo rendering
- QR codes and text will still display normally
- Check serial monitor for BMP loading errors

## Troubleshooting

### Logo doesn't display
- Verify file is named exactly `Header.bmp` (case-sensitive on some systems)
- Check file is in root of SD card (`/Header.bmp`)
- Ensure 24-bit format (not 32-bit with alpha channel)
- Try re-saving with GIMP using settings above

### Colors look wrong
- TFT displays have narrower color gamut than monitors
- Try adjusting brightness/contrast in image editor
- Test with pure colors first (white/black)

### File too large
- Should be ~38KB for 200x64x24-bit
- If larger, likely wrong format (32-bit or compressed)
- Re-save as 24-bit uncompressed BMP

## Sample Code
The ESP32 loads the logo like this:
```cpp
BMPLoader bmpLoader(tft);
if (bmpLoader.draw(systemConfig.headerBMP, 60, 10)) {
  Serial.println("Logo loaded successfully");
} else {
  Serial.println("Logo failed to load - using fallback");
}
```

See [bmp_loader.cpp](../src/display/bmp_loader.cpp) for implementation details.
