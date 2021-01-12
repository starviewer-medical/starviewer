How to add background to a text box:

1. Create a normal text box as desired
2. Select it
3. Filters > Shadows and Glows > Cutout Glow Filter
4. Filters > Filter Editor
5. Change the settings as follows:
   - Offset: Set Delta X and Y to -100
   - Gaussian Blur: 0.01 (this is the lowest possible setting)
   - Flood: select any color and opacity as required
   - Composite - Operator: Destination over
6. Adjust the positioning of the text within and the size of the box by going to the Filter General Settings tab, where you can change the Co-ordinates and Dimensions as required

---------------------------------------------------

How to generate final SVG to insert in LibreOffice:

1. Select one text box with background
2. Context menu > Select Same > Object Type
3. Path > Object to Path
4. For each text box with background:
4.1. Draw a rectangle anywhere
4.2. Fill the rectangle with the same color as the text box background
4.3. Copy the text box
4.4. Select the rectangle
4.5. Edit > Paste Size > Paste Size
4.6. Select the text box and the rectangle and use alignment tools to move the rectangle exactly over the text box
4.7. Move the rectangle down the stack to put it behind the text box (there are buttons in the top toolbar)
5. Filters > Filter Editor...
6. Delete the 8 cutout glows (left column)
7. Save as Simple SVG
