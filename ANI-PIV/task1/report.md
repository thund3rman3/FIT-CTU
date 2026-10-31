# Assignment 1

## Objective

The main objective was to create a dataset of individual cells and binary masks
of their internal structures. The `masks` dataset represents the cell regions;
`masks_pts` contains segmented internal structures treated as candidate
microtubules. The pipeline also counts the detected structures in each cell
crop.

## Input and dataset

Nine microscopy images were processed. The pipeline produced 365 cell crops,
each 170x170 pixels. The grayscale crops, cell masks, internal-structure
masks, and images with cell detections are saved in `Data/cells`,
`Data/masks`, `Data/masks_pts`, and `Data/boxes`.

## Cell segmentation

1. Each input image was loaded and converted to grayscale.
![](../IMGS/loaded0.png)
2. A bilateral filter was applied before segmentation
   (`d=15`, `sigmaColor=75`, `sigmaSpace=100`, `BORDER_REFLECT`). It reduces
   image noise while preserving edges around the cells.

   A power transform with `gamma=2.5` adjusted image intensity.
   ![](../IMGS/bilater+power.png)
3.  CLAHE then
   enhanced local contrast (`clipLimit=2.0`, `tileGridSize=(8, 8)`).
   ![](../IMGS/clahe.png)
4. Otsu's global thresholding produced a binary image, which was inverted for
   subsequent processing.
   ![](../IMGS/segm_inv.png)
5. Morphological opening (erosion followed by dilation) reduced small
   foreground artifacts. External contours were extracted and filtered by
   area (5,000–15,000 pixels), bounding-box height-to-width ratio
   (0.55–1.45), and circularity (greater than 0.75). These shape criteria
   retain approximately round cell candidates while rejecting many unrelated
   objects.
   ![](../IMGS/morph_drawC.png)
6. A 170 × 170 pixel crop was taken around the centre of each accepted
   contour's bounding box. Crops extending beyond the image boundaries were
   skipped. Each grayscale crop is stored in `cells`; its corresponding
   binary cell region is stored in `masks`.
   ![](../IMGS/detect.png)

## Internal-structure masks and counts

Before segmenting internal structures, each cell mask was refined by
morphological opening with an elliptical 9x9 structuring element (8 erosion
iterations and 10 dilation iterations). 
![](../IMGS/cell_segm.png)
For every cell crop, the
`reconstruct` procedure then:

1. Inverted the grayscale crop and eroded it with a 3x3 square kernel for
   7 iterations to create a marker image.
2. Applied morphological reconstruction by dilation, constrained by the
   inverted image. The reconstruction expands marker regions without
   exceeding the constraint image.
3. Thresholded the difference between the inverted image and the
   reconstruction using Otsu's method.
4. Intersected the result with the cell mask and applied morphological
   opening with a 3x3 kernel. The resulting binary images form `masks_pts`.

   ![](../IMGS/microtubul_mask.png)

The number of detected internal structures is calculated directly in the
notebook. For each `masks_pts` image, `find_contours` extracts external
contours; the number of contours is appended to `cnt_pts`. 

## Peak visualization

A 130x130 pixel patch
(`cells[0][22][20:150, 20:150]`) was taken from a cell crop and normalized by
its pixel sum to form a kernel. `cv.filter2D` applied this kernel to the
first grayscale image using `BORDER_REFLECT`, and the response was normalized
to the 0–255 range. 

![](../IMGS/conv_cell.png)

The response image was then processed with morphological
reconstruction using a kernel size of 15 and 5 iterations.

![](../IMGS/recons_conv_peaks.png)

For visualization, pixels where the reconstructed `mask` is greater than 10
are highlighted in the third channel of a copy of the input image. This shows
candidate high-response locations after reconstruction. 

![](../IMGS/peak_detect.png)

## Output

- `Data/cells`: grayscale cell crops.
- `Data/masks`: binary masks of cell regions.
- `Data/masks_pts`: binary masks of segmented internal structures.
- `Data/boxes`: input images with detected cell regions marked.
- `cnt_pts`: per-image, per-cell contour counts held in the notebook.
