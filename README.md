
# JPEG Decoder Project

In this project, we implemented a JPEG decoder in C. It decompresses images compressed in the JPEG format and outputs them in pgm (grayscale) or ppm (color) format, in accordance with the JPEG standard.

------------------------------------------------------------------------

## Task Distribution and Contributions

### Individual Contributions

**EL CHAMAA MOHAMAD**

- Implementation of JPEG header reading
- Development of the BitStream management system
- Design and implementation of PPM file writing
- Organization and distribution of RGB components in the final matrix
- Memory management optimization and code refactoring
- Establishment of software architecture and dependency management

**BENNA YASSER**

- Implementation of the inverse quantization algorithm
- Development of naive and optimized versions of the inverse DCT transformation
- Design and implementation of the up-sampling system
- Contribution to Huffman decoding
- Development of YCbCr to RGB conversion

**TROMBATI MOHAMED**

- Implementation of the inverse zig-zag algorithm
- Development of Huffman decoding
- Design of the block extraction system
- Development of YCbCr to RGB conversion

### Working Methodology

Our approach was based on close and regular collaboration. Work sessions mainly took place in the lab, allowing direct and effective communication between team members.

Task distribution was done according to each member's skills while ensuring a balanced workload. This organization allowed us to progress methodically and minimize potential delays.

------------------------------------------------------------------------
### Dependency Graph

```
jpeg2ppm.c (main)
├── reader.c
│   └── bitsmanip.c
├── output_file.c
├── writter.c
├── traitement_gris.c
│   ├── huffman.c
│   │   └── bitsmanip.c
│   ├── zigzag.c
│   ├── iqzz.c
│   └── idct.c
├── traitement_couleur.c
│   ├── huffman.c
│   │   └── bitsmanip.c
│   ├── zigzag.c
│   ├── iqzz.c
│   ├── idct.c
│   ├── upsampling.c
│   ├── ycbcr_rgb.c
│   └── extraction_bloc.c
│       └── huffman.c
│           └── bitsmanip.c
```

------------------------------------------------------------------------
### Details for Each Module

#### Entry Point and General Orchestration (jpeg2ppm.c)

The main file `jpeg2ppm.c` is the program's entry point. It handles argument parsing, opening the JPEG file, calling color or grayscale decoding functions depending on the number of components, creating the output filename, writing the decoded image in the appropriate format (PGM or PPM), and freeing memory. It orchestrates the entire decoding pipeline by calling specialized modules for each step.

#### BitStream Management (bitsmanip.c)

The bit manipulation module (`bitsmanip.c`) allows precise access to compressed data at the binary level. The implementation is based on a `BitStream` structure that efficiently encapsulates the state of bitstream reading: a file pointer (`fp`), an 8-bit buffer (`buffer`), a counter for available bits (`bits_available`), a counter for bytes read (`bytes_read`), and an end-of-file indicator (`is_eof`). The module's architecture follows a robust encapsulation model with initialization (`bitstream_init`) and cleanup (`bitstream_close`) functions that handle memory allocation/deallocation and file opening/closing. The core functionality lies in the binary reading mechanism, built around four main operations: (1) `bitstream_fill_buffer` which automatically refills the buffer when empty, (2) `bitstream_read_bit` which extracts a single bit from the buffer with proper shifting, (3) `bitstream_read_bits` which composes several bits into a single value via sequential accumulation, and (4) `bitstream_peek_bits` which allows non-destructive reading by saving and restoring the stream state. The implementation also includes an abstraction layer via accessors (`get_fp`, `get_buffer`, etc.) that reinforce data encapsulation and facilitate maintenance. The entire module demonstrates a careful design that balances computational efficiency and structural clarity, providing a reliable interface for bit-by-bit processing of compressed JPEG data.

#### JPEG Header Reading (reader.c)

The JPEG header reading module is responsible for correctly interpreting the complex metadata that precedes the compressed image data. The implementation is structured around a set of specialized functions for parsing the various JPEG marker segments. The `read_jpeg` function orchestrates the overall process, sequentially identifying and processing the SOI (Start Of Image), APP0 (Application data), DQT (Define Quantization Table), SOF0 (Start Of Frame), DHT (Define Huffman Table), SOS (Start Of Scan), and other essential segments. For each segment type, dedicated functions (`read_appx`, `read_sofx`, etc.) meticulously dissect the binary data according to the JPEG standard. The architecture demonstrates a robust approach to the diversity of JPEG encodings: the module precisely manages the extraction and interpretation of crucial parameters such as image dimensions, number of color components, horizontal and vertical sampling factors for each component, quantization tables, and Huffman table specifications. Special care is taken to verify the validity of extracted data, with consistency checks applied to component identifiers, table indices, and other critical parameters. The module also implements memory management to efficiently store the multiple tables and data structures needed for later decoding steps. Quantization tables are organized in two-dimensional arrays, while Huffman code specifications are extracted to facilitate later decoding tree construction. This component illustrates a methodical implementation of the JPEG standard, balancing fidelity to specifications and adaptability to variations found in JPEG files from diverse sources. Its role is crucial in correctly configuring all parameters that determine the success of subsequent decoding steps.

#### Block Extraction (extraction_bloc.c)

The block extraction module is a central step in the JPEG decoding process, responsible for reconstructing the fundamental units of the compressed image. The implementation is based on a main function `extraire_bloc` that methodically orchestrates the decoding of MCUs (Minimum Coded Units) from the compressed data stream. The process begins with a precise analysis of the image's spatial structure by calculating the MCU dimensions (based on the horizontal H_Y and vertical V_Y sampling factors of the luminance component) and determining the total number of MCUs needed to cover the entire image. The adopted memory architecture is hierarchical and adaptive: for each component (Y, Cb, Cr), a three-dimensional structure is dynamically allocated to store the DCT coefficients of each 8×8 block in each MCU. Decoding proceeds sequentially by MCU, successively processing all blocks of each color component. For each block, the function calls `decoder_bloc`, which uses the Huffman tables specific to each component and coefficient type (DC or AC) to reconstruct the quantized values. A crucial aspect of the algorithm is the management of DC coefficients, which are encoded differentially: the module therefore maintains state variables (`dc_prec_Y`, `dc_prec_Cb`, `dc_prec_Cr`) to ensure decoding consistency between successive blocks. The implementation pays particular attention to memory optimization through allocation sized precisely to the exact number of MCUs and blocks required for the image.

#### Huffman Decoding (huffman.c)

The Huffman decoding module is a critical component of the JPEG decoder, responsible for transforming compressed bitstreams into meaningful symbols. The implementation leverages the nature of variable-length codes, where frequent symbols are represented by short codes and rare symbols by longer codes. The module's core structure is organized around two main functions: building the decoding tables and extracting symbols from the bitstream. The `generer_codes_huffman` function systematically analyzes the code length specifications and code values provided in the JPEG header to generate efficient lookup tables. These tables are structured as strings representing binary codes, with dynamic allocation adapted to the variable length of each code. The second main function, `decoder_bloc`, uses these tables to sequentially extract the DCT coefficients of each 8×8 block. This function handles two distinct types of coefficients: DC coefficients, encoded differentially (the difference with the DC coefficient of the previous block), and AC coefficients, which may be followed by zero sequences (RLE - Run Length Encoding). Decoding proceeds by bit-by-bit analysis of the incoming stream: at each step, the decoder attempts to match the current bit sequence with a valid Huffman code. Once a match is found, the associated symbol is interpreted, either directly (for low-amplitude AC coefficients) or as an indication of the bit length of the next value (especially important for DC coefficients). The implementation also includes a sophisticated mechanism for handling special cases such as the End Of Block (EOB) symbol, which signals that all remaining AC coefficients are zero, as well as special codes indicating zero runs. This design efficiently balances decoding speed and accuracy, enabling faithful reconstruction of the original image data from the compressed stream.

#### Inverse DCT Transformation (idct.c)

The inverse discrete cosine transform (IDCT) module is the mathematical backbone of the JPEG decoder, responsible for converting frequency coefficients into pixel values in the spatial domain. The implementation offers two complementary approaches: a naive version and an optimized version. The naive version (`idct_naive`) faithfully implements the mathematical formula of the 2D inverse DCT as four nested loops, calculating for each spatial position (x,y) a weighted sum of the 64 frequency coefficients, with application of specific normalization factors (`coeff_c`) for zero-frequency coefficients. While didactic and exact, this implementation has an algorithmic complexity of O(n⁴), making it unsuitable for performance-critical applications. To address this, the module offers a highly optimized implementation based on the Loeffler algorithm (`idct_rapide`), which decomposes the 2D transform into a series of 1D transforms applied successively to rows and then columns. This approach exploits the separability of the DCT and reduces complexity to O(n² log n). The Loeffler algorithm subdivides each 1D transform into four distinct steps, carefully implemented in dedicated functions: (1) an initial rearrangement of coefficients (`etape1_idct_rapide`) that prepares calculations according to a specific non-sequential scheme, (2) a first series of "butterfly" operations (`etape2_idct_rapide`) with precomputed trigonometric rotations, (3) a second series of butterfly operations (`etape3_idct_rapide`) with more complex rotations, and (4) a final reorganization step (`etape4_idct_rapide`). Optimizations include precomputing trigonometric coefficients (sines and cosines at various angles such as π/16, 3π/16, π/8), factoring common expressions into intermediate variables (`calcul`, `calcul1`, `calcul2`), and using intermediate buffers (`buff1`, `buff2`, `tmp`) to minimize memory allocations. The module also precisely handles critical aspects of the transform such as normalization by the constant INV_SQRT2 (1/√2) and the final adjustment of spatial values (multiplication by 8 and addition of 128) to bring them into the [0,255] pixel range, with explicit saturation of extreme values. This implementation efficiently balances numerical precision and computational performance, making it an essential component of the decoder's overall efficiency.

#### Inverse Quantization (iqzz.c)

The inverse quantization module is a fundamental step in the JPEG decoding chain, responsible for restoring the amplitude of DCT coefficients that were reduced during compression. The implementation is characterized by its elegant simplicity and algorithmic efficiency. The `quantification_inverse` function operates directly on a linearized block of 64 DCT coefficients and uses a specific quantization table provided as a parameter. The underlying mathematical principle is a simple term-by-term multiplication: each DCT coefficient is multiplied by the corresponding value in the quantization table. This operation reverses the division performed during compression, gradually restoring the image details that were altered by quantization. The block's implicit two-dimensional structure is preserved by appropriate index calculation (`i * N + j`), allowing access to the corresponding elements in the linear array. The implementation has optimal algorithmic complexity of O(n²), with exactly 64 multiplications for a standard 8×8 block, and no superfluous operations. This module works in tandem with the inverse zig-zag module: typically, coefficients are first ordered according to the conventional zigzag order, then passed to this function for de-quantization. Numerical precision is maintained by using the `int` type for coefficients, providing sufficient dynamic range to accommodate the expansion of values resulting from multiplication by quantization factors, which are coded on 16 bits (`uint16_t`). This approach ensures that essential frequency information, especially low-frequency components crucial for perceived image quality, is faithfully restored before applying the inverse DCT. The module demonstrates a straightforward and direct design, favoring efficiency and clarity, while being an indispensable link in reconstructing the compressed image.

#### Inverse Zigzag Transformation (zigzag.c)

The inverse zigzag transformation module is a fundamental step in the JPEG decoding process, responsible for spatially reconstructing DCT coefficients previously linearized according to a compression-optimized path. The implementation is built around the `inverse_zigzag` function, which methodically transforms a one-dimensional vector of 64 elements into an 8×8 two-dimensional matrix, thus restoring the original spatial arrangement of frequency coefficients. The core algorithm exploits an elegant mathematical property: all elements on the same diagonal share the same value for the sum of their row and column indices. The algorithm proceeds by sequentially traversing the 15 possible diagonals (represented by the sum row+column varying from 0 to 14), systematically alternating between two types of trajectories depending on the parity of this sum. When the sum is even, the module traverses the diagonal from bottom to top (bottom-left to top-right), while when it is odd, it traverses from top to bottom (top-left to bottom-right). This bidirectional logic exactly mirrors the inverse of the zigzag path used during compression, which concentrates non-zero coefficients at the start of the sequence. The implementation is remarkably concise, with only four control variables (`i` for the vector index, `row` and `col` for matrix coordinates, and `slc` for the row+column sum), structured around two nested loops that select the appropriate traversal direction based on parity. Boundary checks (`row < 8 && col < 8`) ensure that only valid positions in the 8×8 matrix are accessed. This transformation is essential for preparing coefficients for the inverse quantization and then the inverse DCT, thus directly contributing to the faithful reconstruction of the original image from its compressed representation.

#### Output File Management (output_file.c)

The output file management module is an auxiliary component of the JPEG decoder, ensuring a smooth transition between input and output formats. The implementation focuses on the `create_output_filename` function, which transforms the input JPEG file path into an appropriate filename for the decoded image. This function operates according to precise string manipulation logic: it first locates the last occurrence of a dot in the filename (indicating the extension) and checks that this dot does not belong to a directory (by comparing its position with the last path separator). Then, depending on a flag indicating the type of image to produce: for color images (flag==1), the extension is replaced by ".ppm" (Portable PixMap), while for grayscale images, it becomes ".pgm" (Portable GrayMap). The implementation includes memory management with dynamic allocation sized to accommodate the new filename, and appropriate checks to avoid memory leaks in case of allocation failure. This module illustrates a pragmatic design that facilitates the decoder's workflow by standardizing output file naming while preserving information about their nature (color or grayscale) directly in the extension.

#### Writing Decoded Images (writter.c)

The image writing module is responsible for the persistence and structured presentation of reconstructed image data. The implementation is built around a three-level architecture that encapsulates the complete lifecycle management of decoded images. The central `IMAGE_D` structure serves as a polymorphic container capable of holding both grayscale and color images, with memory organization optimized according to the image type: for grayscale images, only the `R` component is allocated and used as the luminance channel, while for color images, all three components `R`, `G`, and `B` are fully used. The `init_image_d` function ensures precise dynamic allocation of memory resources adapted to the image's dimensions and format, with validity checks to guarantee program robustness. At the other end of the lifecycle, the `free_image_d` function ensures complete release of allocated resources, thus preventing potential memory leaks by adapting its behavior to the image mode (grayscale or color). There are two complementary operations: `copy_mcu_to_image`, which ensures precise integration of decoded 8×8 blocks (or MCUs of variable dimensions depending on the sampling format) into the global image structure, taking into account spatial coordinates and checking boundaries to avoid overflows, and `write_image_d`, which finalizes the process by generating the image file in the standardized PGM (for grayscale images) or PPM (for color images) format, in accordance with the format specifications. This last function precisely implements the specific syntax of these formats, including the header with dimensions and color depth, followed by sequentially arranged pixel data. The entire module demonstrates a design that balances memory efficiency and structural clarity, offering a consistent interface between the internal decoding process and the standardized external representation of images.

#### Color Processing (traitement_couleur.c)

The color processing module is the main orchestrator of the decoding pipeline for color JPEG images, harmoniously integrating the various transformation steps to produce the final image. The implementation is built around the `traitement_color` function, which supervises the entire colorimetric decompression process by methodically coordinating operations on the three Y, Cb, and Cr components. The process begins with a detailed analysis of the horizontal and vertical sampling factors of each component to precisely determine the subsampling format used (4:4:4, 4:2:2 horizontal, 4:2:2 vertical, or 4:2:0), with validity checks to ensure format compatibility with the decoder's capabilities. The module's architecture reflects the complete decoding sequence: after building the appropriate Huffman tables for each coefficient type (DC/AC) and component (luminance/chrominance), the module precisely calculates the dimensions and number of MCUs needed to reconstruct the image. For each MCU, a systematic process is applied to the extracted DCT coefficient blocks: inverse quantization, inverse zigzag reordering, then application of the inverse DCT to obtain spatial values. Special attention is paid to the differentiated processing of chrominance components, which, depending on the identified subsampling format, are subjected to specific up-sampling operations via appropriate functions (`up_sampling4_2_2_horizontal`, `up_sampling4_2_2_vertical`, or `up_sampling4_2_0`). The crucial color conversion step is then performed by the `YCbCr_to_rgb` function, which transforms YCbCr values into conventional RGB components, followed by integration of the resulting data into the final image structure via `copy_mcu_to_image`. The implementation demonstrates meticulous memory management with targeted dynamic allocations for each intermediate structure and systematic releases after use, thus preventing memory leaks despite the complexity of the four-level pointer structures. This module illustrates the sophisticated integration of multiple JPEG decoder algorithms into a coherent and efficient sequence, while adapting to the variations in subsampling formats that characterize different chromatic compression strategies.

#### Grayscale Image Processing (traitement_gris.c)

The grayscale image processing module is a streamlined and optimized implementation of the JPEG decoding pipeline for monochrome images. The implementation is built around the `traitement_grayscale` function, which sequentially executes the essential decompression steps with a simplified architecture compared to color processing. The process begins with the initialization of critical parameters and precise calculation of the image dimensions in terms of MCUs (Minimum Coded Units), thus determining the total number of 8×8 blocks needed to reconstruct the complete image. A distinctive feature of this implementation is its handling of a single luminance (Y) component, thus eliminating the complexity inherent in chromatic subsampling formats. The actual decoding sequence begins with the generation of Huffman tables adapted to the DC and AC components from the specifications extracted from the JPEG header. For each MCU, the module methodically applies four fundamental transformations: (1) decoding the compressed stream via the `decoder_bloc` function, which uses the Huffman tables to reconstruct the 64 quantized DCT coefficients, (2) inverse quantization to restore the amplitude of the frequency coefficients, (3) inverse zigzag reordering to arrange the coefficients into an 8×8 matrix according to their original spatial arrangement, and (4) application of the optimized inverse DCT to convert the frequency coefficients into pixel values in the spatial domain. Each decoded block is immediately integrated into the final image structure via the `copy_mcu_to_image` function, with meticulous memory management that systematically frees temporary allocations after each block is processed. This sequential approach optimizes the decoder's memory footprint even for large images. The implementation stands out for its conciseness and efficiency, favoring a direct approach that eliminates superfluous steps while maintaining fidelity to the original image, making it an ideal solution for applications requiring fast processing of JPEG grayscale images.

#### Chromatic Upsampling (upsampling.c)

The `upsampling.c` module implements the various upsampling methods needed to convert subsampled chrominance components Cb and Cr into matrices of size compatible with luminance Y. It provides dedicated functions for each common JPEG sampling format: `up_sampling4_2_2_horizontal` for 4:2:2 horizontal, `up_sampling4_2_2_vertical` for 4:2:2 vertical, and `up_sampling4_2_0` for 4:2:0. Each function dynamically allocates the output matrices and duplicates chrominance values according to the sampling scheme, thus enabling correct color recombination during YCbCr to RGB conversion.

#### YCbCr to RGB Conversion (ycbcr_rgb.c)

The YCbCr to RGB conversion module is an essential component of the JPEG decoder for color images, ensuring the final transition from the colorimetric format used in compression to the conventional RGB space for display. The implementation is built around the `YCbCr_to_rgb` function, which operates on blocks of luminance (Y) and chrominance (Cb, Cr) components to generate the three corresponding RGB component matrices. The core algorithm intelligently adapts its behavior depending on the detected sampling format (4:2:0, 4:2:2 vertical, or 4:2:2 horizontal), with a global coordinate system that precisely calculates the position of each pixel in the final color space. For each sampling type, the module applies specific mapping logic: in the case of 4:2:0, global coordinates are calculated based on block position and local coordinates; for 4:2:2 vertical and horizontal, different formulas are applied to correctly access the subsampled chrominance components. The actual conversion implements the standard YCbCr to RGB transformation equations with optimized coefficients (1.402 for Cr to R, -0.34414 for Cb to G, -0.71414 for Cr to G, and 1.772 for Cb to B), while integrating a -128 offset for Cb and Cr components to bring them into the appropriate range [-128, 127]. A saturation mechanism is systematically applied to the resulting RGB values, ensuring they remain strictly within the valid [0, 255] range despite potential numerical inaccuracies inherent in floating-point calculations. The module's memory architecture reveals careful management with dynamic allocations adapted to the image dimensions, organized in a matrix structure that facilitates sequential access during subsequent processing. This implementation is the crucial final step that transforms the decompressed and processed data into a faithful and display-ready visual representation, thus completing the decoding pipeline for color JPEG images.

------------------------------------------------------------------------
### General Structure and Program Flow

The program follows a typical sequential JPEG decoder pipeline:
1. Reading and analyzing JPEG headers to extract necessary parameters and tables.
2. Huffman decoding and extraction of DCT coefficient blocks for each component.
3. Inverse quantization and inverse zigzag reordering of coefficients.
4. Application of the inverse DCT transformation (IDCT) to obtain spatial values.
5. Upsampling of chrominance components if necessary.
6. Conversion of YCbCr components to RGB (for color images).
7. Construction of the final image and writing in PGM or PPM format.

Each step is handled by a dedicated module, promoting clarity, modularity, and maintainability of the code.

------------------------------------------------------------------------
### Additional Information

We have verified that memory allocation and deallocation are properly managed for processing images that respect the required format and specifications (memory leak = 0).
However, for images that cause errors, no `on_exit()` function has been defined to free all allocated memory, which may result in memory leaks in these particular cases.

There are counter-examples in the `ne_marche_pas` directory.
