<h1 align="center">QuadTree Image Compression Tool</h1>
<p align="center"><em>Tugas Kecil 2 IF2211 Strategi Algoritma 2025</em></p>

<p align="center">
  <img src="test/gif/stei_3_25.gif" width="200">
</p>
<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17">
</p>

<h2> About This Project</h2>

<p>Using C++, this program implements the <strong>divide and conquer</strong> paradigm to recursively partition images. Areas of the image exhibiting color homogeneity are efficiently represented using their average color, stored within a <strong>QuadTree</strong> structure.</p>

<p>This serves as a fundamental yet effective approach to lossy image data reduction, developed as part of the IF2211 Algorithmic Strategies course assignment.</p>



<h2> Project Structure</h2>

<pre>
.
├── bin
├── doc
├── README.md
├── src                
│   ├── main.cpp           
│   ├── QuadTree.hpp       
│   ├── QuadTree.cpp      
│   ├── QuadNode.hpp      
│   ├── Image.hpp          
│   ├── stb_image.h        
│   ├── stb_image_write.h  
│   ├── stb_image.cpp      
│   └── gif.h              
└── test                 
    └── gif/
    └── image/
              
</pre>

<h2> Getting Started</h2>

<h3> Prereq</h3>
<ul>
  <li><strong>C++17</strong> standard or newer (e.g., g++ 7+, Clang 5+, MSVC 2017+).</li>
  <li><em>Bundled Header-Only Libraries:</em> This project uses <code>stb_image.h</code>, <code>stb_image_write.h</code>, and <code>gif.h</code>, which are included directly within the <code>src/</code> directory. No external installation is required for these.</li>
</ul>

<h3> Installation & Compilation</h3>
<ol>
  <li><strong>Clone the Repository:</strong>
    <pre><code class="lang-bash">git clone https://github.com/poetoeee/Tucil2_13523096
cd Tucil2_13523096</code></pre>
  </li>
  <li><strong>Compile the Source Code (Example using g++):</strong>
    <p>Navigate to the project's root directory via your terminal, then run:</p>
    <pre><code class="lang-bash">gg++ -std=c++17 -I./src src/main.cpp src/QuadTree.cpp src/stb_image.cpp -o bin/main
./bin/main</code></pre>
  </li>
</ol>




<h2>Author</h2>
<ul>
  <li>Muhammad Edo Raduputu Aprima (13523096)</li>
</ul>

---
<p align="center">Selamat Mengompres!</p>
