# Test-Viewmatrix
just give the View Matrix address and PID and if Row major or not
After that an ESP with a small cirle should appear in world Center (0,0,0)


<img width="702" height="239" alt="image" src="https://github.com/user-attachments/assets/b0e74eb3-9a89-497b-8e08-1ce883f7162b" />


## If you cant set up OpenGL
Download this:
https://drive.google.com/drive/folders/13MT8TGidc-La94b0VsUDCYgEXUYzPsnc?usp=sharing

Everything except for " assimp-vc143-mt.dll " should be inside
source > repos > openGL viewmatrix tester  >openGL viewmatrix tester

and " assimp-vc143-mt.dll " should be inside:
C:\Users\username\source\repos\openGL viewmatrix tester\x64\Debug or Release

After that do this:
<img width="957" height="463" alt="image" src="https://github.com/user-attachments/assets/40602c88-7eaf-402a-bb42-2e54802df6de" />

<img width="1005" height="729" alt="image" src="https://github.com/user-attachments/assets/2b430a05-089f-4592-967e-32c1898b248b" />

<img width="575" height="1192" alt="Screenshot 2026-06-27 180721" src="https://github.com/user-attachments/assets/b5ed6394-82d9-4548-840f-a64d6d14e2d3" />

### ➤ Additional Include Directories
`C/C++ → General`

```
$(ProjectDir)include
```

---

### ➤ Additional Library Directories
`Project → Properties → Linker → General`

```
$(ProjectDir)libs\glfw\lib-vc2022
$(ProjectDir)libs\assimp\lib

```

```
$(ProjectDir)libs\glfw\lib-vc2022;$(ProjectDir)libs\assimp\lib
```

---

### ➤ Linker Dependencies
`Linker → Input → Additional Dependencies`

```
glfw3.lib
assimp-vc143-mt.lib

```
