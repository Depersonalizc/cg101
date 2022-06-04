## 3D Transformations (II)

Previously, we have discussed linear and affine transformations in $\R^3$ and their matrix representations using homogeneous coordinates. Now is the time to discuss their applications in 3D viewing together with a final class of powerful transformations: projections. $\newcommand {\b}{\mathbf} \newcommand{\para}{\mathbin{\!/\mkern-5mu/\!}}$

### 3D viewing

A (geometric) model of an object can be stored in the computer as a list of 3D vertices $\b V =\{\b v_i\}$, faces as groupings of vertices, and face normals $\b N=\{\hat{\b n}_j\}$ specifying the normal direction of each face. On a high level, viewing is a transformation $\mathcal V:\R^3\to \R^3$ that maps $\b V$ and $\b N$ to a "canonical" space named normalized device coordinates (NDC). The viewing transformation can be written as a composition of

1. **Model** transformation $\mathcal M$
2. **Camera (View)** transformation $\mathcal C$
3. **Projection** transformation $\mathcal P$,

in that $\mathcal V=\mathcal P\circ\mathcal C\circ\mathcal M$. To understand all these transforms, it helps conceptually to imagine a virtual camera that captures a scene. Say we want to take a photo of a toy Teddy Bear. What we have to do are the following:

1. Place Ted in a nice spot and pose (say close to the window, rotated a bit so his left body is lit by the Sun)
2. Pick a right place and angle for the camera (say pointing straight to Ted in a full body portrait)
3. Install the right lens onto the camera and "Cheese!"

These three steps basically correspond to the three transforms introduced above, as we shall see next.

### Model transformation

The model transformation $\mathcal M$ is an affine transformation that takes a model to wherever you want it to be in the scene. It also rotates the model in any direction you want, stretching and shearing it if necessary (poor Ted). As we saw last time such a transformation can be expressed as a 4x4 matrix of the form
$$
\b M=\left[\begin{array}{c|c}
\b T_{3\times 3} & \b t\\
\b 0^\top & 1

\end{array}\right]
$$
encoding the transform
$$
\begin{aligned}
\mathcal M(\b v) &= \b T\b v+\b t  & \text{for vertex }\b v
\\
\mathcal M(\b d) &= \b T\hat{\b d}   &\text{for direction }\hat{\b d}

\end{aligned}
$$
Now suppose Ted feels a bit lonely and wants his buddy John in the photo next to him. We can use another model transformation $\mathcal M'$ to transform John's vertices and normals to where Ted wants him to be. In general, each model in a scene has its own model transform that mapping the vertices in its local coordinates to a shared, world coordinate space.
$$
\left\{
\b M^{(1)}=\left[\begin{array}{c|c}
\b T^{(1)} & \b t^{(1)}\\
\b 0^\top & 1

\end{array}\right],...,\ \b M^{(n)}=\left[\begin{array}{c|c}
\b T^{(n)} & \b t^{(n)}\\
\b 0^\top & 1

\end{array}\right]\right\}
$$
The coordinates system after applying model transform is called the *world space coordinates*.

### Camera (View) transformation

With Ted and his friends ready, time to take out the camera. The camera or view transform $\mathcal C$, as the name suggests, effectively "puts the camera at a nice angle". To fully determine a camera's orientation in space three parameters are needed:

- Position $\b e$  (Where is the camera?)
- Look-at / gaze direction $\hat {\b g}$  (Which direction is the camera pointing to?)
- Up direction $\hat{\b t}$  (Which direction is the top of the camera pointing to?)

The camera transformation is defined as the rigid transformation mapping the vectors
$$
\begin{aligned}
&\ \ \cal C\\
\b e &\mapsto \b 0\\
\b e+\hat{\b t}&\mapsto \hat{\b y}\\
\b e+\hat{\b g}&\mapsto -\hat{\b z}
\end{aligned}
$$
Equivalently, it can be understood as a change of coordinates from the $[\hat{\b x},\hat{\b y},\hat{\b z}]$ system to the new system $\b Q = [\hat{\b g}\times\hat{\b t},\hat{\b t},-\hat{\b g}]$ centered at $\b e$. In plain English, we want the camera to be located at the origin, looking at $-\hat{\b z}$, with its "up" direction aligned with $\hat{\b y}$.

<img src="C:\Users\chen1\AppData\Roaming\Typora\typora-user-images\image-20220603221837105.png" alt="image-20220603221837105" style="zoom: 50%;" />

Let's derive a matrix representation for $\mathcal C$. We first translate $\b e$ to $\b 0$ with
$$
\b T =\left[\begin{array}{c|c}
\b I & -\b e\\
\b 0^\top & 1
\end{array}\right]
$$
Next, we need a rotation matrix $\b R$ to turn $\hat{\b t}$ to $ \hat {\b y}$ and $\hat{\b g}$ to $-\hat {\b z}$. The inverse matrix is easy to write:
$$
\b R^{-1}=
\left[\begin{array}{c|c}
{\b Q}&\b 0\\
 \b 0^\top& 1

\end{array}\right]=
\left[\begin{array}{ccc|c}
\hat{\b g}\times\hat{\b t}& \hat{\b t}& -\hat{\b g}&\b 0\\
 0& 0&0& 1

\end{array}\right]
$$
This is an orthogonal matrix. Thus
$$
\b R=(\b R^{-1})^\top =\left[\begin{array}{c|c}
{\b Q^\top }&\b 0\\
 \b 0^\top& 1

\end{array}\right]
$$
from which we get
$$
\b C = \b R\b T=\left[\begin{array}{c|c}
{\b Q^\top }&\b 0\\
 \b 0^\top& 1

\end{array}\right]\left[\begin{array}{c|c}
\b I & -\b e\\
\b 0^\top & 1
\end{array}\right]=\left[\begin{array}{c|c}
\b Q^\top & -\b Q^\top \b e\\
\b 0^\top & 1
\end{array}\right]
$$
which encodes the transform
$$
\begin{aligned}
\mathcal C(\b v) &= \b Q^\top(\b v-\b e)  & \text{for vertex }\b v
\\
\mathcal C(\b d) &= \b Q^\top\hat{\b d}   &\text{for direction }\hat{\b d}

\end{aligned}
$$
The coordinates system after applying model and camera transforms is called the *camera space coordinates*.

### Projection transformation

The (perspective) projection transformation $\mathcal P$ "squishes", translates and scales a so-called the *view frustum* to the canonical cube $K=[-1,1]^3$. The view frustum is a right, rectangular frustum in front of the camera whose height is parallel to the $z$ axis of the camera space. 

Denote the rectangular face of the frustum closer to the camera by $N$ and the father one $F$. The frustum, and thus transform $\mathcal P$, can be determined by the following parameters:

| Parameter | $n$ (near) | $f$ (far) | $t$ (top)  | $b$ (bottom) | $r$ (right) | $l$ (left) |
| --------- | ---------- | --------- | ---------- | ------------ | ----------- | ---------- |
| means...  | $-N_z$     | $-F_z$    | $\max N_y$ | $\min N_y$   | $\max N_x$  | $\min N_x$ |

<img src="C:\Users\chen1\AppData\Roaming\Typora\typora-user-images\image-20220604032511065.png" alt="image-20220604032511065" style="zoom:33%;" />

#### "Squishing"

Let's assume the squishing transform $\mathcal S$ can be represented by a 4x4 matrix $\b S$. We shall derive what $\b S$ should be. We wish to map the view frustum to the cuboid $C=[l,r]\times [b,t]\times[-f,-n]$. Additionally, we require the transform $\mathcal S$ to

- scale the $x$ and $y$ coordinates of each points so that every cross section of the frustum after transformation has the same dimension as $N$, and
- keep the $z$ coordinates of the near and far planes unchanged.

<img src="C:\Users\chen1\AppData\Roaming\Typora\typora-user-images\image-20220604042034369.png" alt="image-20220604042034369" style="zoom:33%;" />

By inspecting two sets of similar triangles above, we know $\mathcal S$ should scale each $x$-$y$ plane uniformly by a factor $-n/z$ where $z$ is the $z$ coordinate of the plane. That is,
$$
\mathcal S(\b v)=\mathcal S(v_x,v_y,v_z)=\left(\frac{-nv_x}{v_z},\frac{-nv_x}{v_z},v_z'\right)
$$
In homogeneous coordinates,
$$
\b S \tilde {\b v}= \b S\left[\begin{array}{c}
v_x\\v_y\\v_z\\1
\end{array}\right]\sim \left[\begin{array}{c}
-nv_x/v_z\\-nv_y/v_z\\v'_z\\1
\end{array}\right]\sim\left[\begin{array}{c}
-nv_x\\-nv_y\\v'_zv_z\\v_z
\end{array}\right]
$$
Thus we know $\b S$ must have the form
$$
\b S =\left[\begin{array}{ccc}
-n&0&0&0
\\
0&-n&0&0
\\
a&b&c&d
\\
0&0&1&0
\end{array}\right]
$$
where $a,b,c,d$ are unknown. To find out these unknowns, observe that after squishing

1. The entire near plane remains unchanged
2. The intersection point on the far plane $(0, 0, -f)$ remains unchanged

The first condition implies that
$$
\b S\left[\begin{array}{c}
v_x\\v_y\\-n\\1
\end{array}\right]=\left[\begin{array}{c}
-nv_x\\-nv_y\\av_x+bv_y-cn+d\\-n
\end{array}\right]\sim \left[\begin{array}{c}
v_x\\v_y\\\frac{av_x+bv_y-cn+d}{-n}\\1
\end{array}\right]=\left[\begin{array}{c}
v_x\\v_y\\-n\\1
\end{array}\right]
$$
for all $v_x,v_y$. Hence $a=b=0$ and $d-cn=n^2$.

The second condition implies
$$
\b S\left[\begin{array}{c}
0\\0\\-f\\1
\end{array}\right]=\left[\begin{array}{c}
0\\0\\-cf+d\\-f
\end{array}\right]\sim \left[\begin{array}{c}
0\\0\\\frac{-cf+d}{-f}\\1
\end{array}\right]=\left[\begin{array}{c}
0\\0\\-f\\1
\end{array}\right]
$$
Hence $d-cf=f^2$. Solving the above system yields
$$
{a=b=0\\c=-(n+f),d=-nf}
\implies
\b S =\left[\begin{array}{ccc}
-n&0&0&0
\\
0&-n&0&0
\\
0&0&-(n+f)&-nf
\\
0&0&1&0
\end{array}\right]
$$

#### Translation & Scaling

Now that the frustum has been squished to the cuboid $C$, all there's left to do is map the cuboid to the canonical cube $K$. This is straightforward to accomplish. First, translate the center of the cuboid $(\frac{l+r}{2},\frac{t+b}{2},-\frac{f+n}{2})$ to the origin. Then stretch along each axis (respectively by $\frac{2}{r-l},\frac{2}{t-b},\frac{2}{f-n}$) so that all side-lengths of the cuboid become $2$. The overall transform is given by the matrix
$$
\b A =\left[\begin{array}{ccc}
\frac{2}{r-l}&0&0&\frac{l+r}{l-r}
\\
0&\frac{2}{t-b}&0&\frac{b+t}{b-t}
\\
0&0&\frac{2}{f-n}&\frac{f+n}{f-n}
\\
0&0&0&1
\end{array}\right]
$$

#### Putting it together

The projection matrix is therefore the product
$$
\b P = \b A\b S
=\left[\begin{array}{ccc}
\frac{-2n}{r-l}&0&\frac{l+r}{l-r}&0
\\
0&\frac{-2n}{t-b}&\frac{b+t}{b-t}&0
\\
0&0&\boxed{\frac{n+f}{n-f}}&\frac{2nf}{n-f}
\\
0&0&\boxed{1}&0
\end{array}\right]\label {proj}
$$
The coordinates system after applying the model, camera, and projection transformations is called the *normalized device coordinates* (NDC).

*Note*: In some graphics API's like OpenGL, the NDC system is defined to be left-handed (camera pointing to $\hat{\b z}$ instead of $-\hat{\b z}$) so that a larger $z$ coordinate in NDC corresponds to greater depth from the camera. To get this type of projection matrix, simply negate the boxed elements in $(\ref {proj})$.



### References

[1] https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html

[2] http://www.songho.ca/opengl/gl_projectionmatrix.html

---

**Next:** Rasterization

*To be continued.*



