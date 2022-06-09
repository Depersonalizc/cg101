## 3D Transformations (II)

Previously, we have discussed linear and affine transformations in $\R^3$ and their matrix representations using homogeneous coordinates. Now is the time to discuss their applications in 3D viewing together with a final class of powerful transformations: projections. $\newcommand {\b}{\mathbf} \newcommand{\para}{\mathbin{\!/\mkern-5mu/\!}}$

### 3D viewing

A (geometric) model of an object can be stored in the computer as a list of 3D vertices $\b V =\{\b v_i\}$, faces as groupings of vertices, and face normals $\b N=\{\hat{\b n}_j\}$ specifying the normal direction of each face. On a high level, viewing is a transformation $\mathcal V:\R^3\to \R^3$ that bijectively maps $\b V$ and $\b N$ to a "canonical" space named the normalized device coordinates (NDC). The viewing transformation can be written as a composition of

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
\mathcal M(\hat {\b n}) &= \b T\hat{\b n}   &\text{for normal }\hat{\b n}

\end{aligned}
$$
... Right? **Sadly NO**. The matrix $\b M$ works wonderfully at transforming the vertices. However, it fails to keep the normals perpendicular to the faces after transformation. To see this, consider a face $F$ and any two distinct points $\b u,\b v$ on it. The defining property of the surface normal $\hat {\b n}$ is that $\hat {\b n}^\top(\b u-\b v)=0$. After the transformation we wish the new normal vector $\hat {\b n}'$ still be perpendicular to the transformed face $F'$.

<img src="assets/02_3D Transformations II/model_normal.png" style="zoom:33%;" />

In other words,
$$
\begin{aligned}
(\hat {\b n}')^{^\top}(\mathcal M(\b u)-\mathcal M(\b v))=0\\
(\hat {\b n}')^{^\top}\b T(\b u-\b v)=0

\end{aligned}
$$
It is not too hard to see that $\hat {\b n}'=\b T^{-\top}\hat {\b n}$ is the solution, since
$$
(\b T^{-\top}\hat {\b n})^{^\top}\b T(\b u-\b v)=\hat {\b n}^\top\b T^{-1}\b T(\b u-\b v)=0
$$
So the correct transformation is actually
$$
\begin{aligned}
\mathcal M(\b v) &= \b T\b v+\b t  & \text{for vertex }\b v
\\
\mathcal M(\hat {\b n}) &= \b T^{-\top}\hat{\b n}   &\text{for normal }\hat{\b n}

\end{aligned}
$$
Now, suppose Ted feels a bit lonely and wants his buddy John in the photo next to him. We can use another model transformation $\mathcal M'$ to transform John's vertices and normals to where Ted wants him to be. In general, each model in a scene has its own model transform that mapping the vertices in its local coordinates to a shared, world coordinate space.
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
The coordinates system after applying the model transform is called the *world space coordinates*.

### Camera (View) transformation

With Ted and his friends ready, time to take out our camera. 

The camera (or view) transformation $\mathcal C$, as the name suggests, effectively puts our virtual camera at a nice angle. To fully determine the camera's orientation in space three parameters are needed:

- Position / "Eye" $\b e$  (Where is the camera?)
- Look-at / Gaze direction $\hat {\b g}$  (Which direction is the camera pointing to?)
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

<img src="assets/02_3D Transformations II/cam_tsfm.png" alt="cam_tsfm" style="zoom: 50%;" />

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
This time we don't have to worry about treating the normals separately. Since $\b Q$ is orthogonal,
$$
{(\b Q^{\top})}^{-\top}=\b Q^{-1}=\b Q^{\top}
$$
So the camera transform is simply
$$
\begin{aligned}
\mathcal C(\b v) &= \b Q^\top(\b v-\b e)  & \text{for vertex }\b v
\\
\mathcal C(\b d) &= \b Q^\top\hat{\b n}   &\text{for normal }\hat{\b n}

\end{aligned}
$$
which can be performed with a single matrix $\b C$.

The coordinates system after applying model and camera transforms is called the *camera (or view) space coordinates*.

### Projection transformation

The (perspective) projection transformation $\mathcal P$ "squishes", then translates and scales a so-called the *view frustum* to the canonical cube $K=[-1,1]^3$. The view frustum is a right rectangular frustum in front of the camera whose two rectangular faces are perpendicular to the $z$ axis in the camera space. 

Denote the rectangular face of the frustum closer to the camera by $N$ and the father one $F$. The frustum, and thus transform $\mathcal P$, can be determined by the following parameters:

| Parameter | $n$ (near, $\gt 0$) | $f$ (far, $\gt 0$) | $t$ (top)  | $b$ (bottom) | $r$ (right) | $l$ (left) |
| --------- | ------------------- | ------------------ | ---------- | ------------ | ----------- | ---------- |
| means...  | $-N_z$              | $-F_z$             | $\max N_y$ | $\min N_y$   | $\max N_x$  | $\min N_x$ |

<img src="assets/02_3D Transformations II/view_frustum.png" alt="view_frustum" style="zoom:33%;" />

#### "Squishing" the frustum

Let's assume the "squishing" transform $\mathcal S$ that maps the view frustum to the cuboid $C=[l,r]\times [b,t]\times[-f,-n]$ can be represented by a 4x4 matrix $\b S$. We shall derive a formula for $\b S$ by requiring the transform $\mathcal S$ to

- scale the $x$ and $y$ coordinates of the points on each (rectangular) cross section of the frustum, so that all cross sections have the same dimension as $N$ after the transformation, and
- keep the $z$ coordinates of the near and far planes unchanged.

<img src="assets/02_3D Transformations II/squish_sim.png" alt="squish_sim" style="zoom:33%;" />

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
2. The intersection point on the far plane $(0, 0, -f)$ remains still

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
\begin{aligned}a&=b=0\\-c&=n+f\\-d&=nf\end{aligned}
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

Now that the frustum has been squished to the cuboid $C$, all there's left to do is map the cuboid to the canonical cube $K$. This is rather straightforward to accomplish: First, translate the center of the cuboid $(\frac{l+r}{2},\frac{t+b}{2},-\frac{f+n}{2})$ to the origin. Then stretch along each axis (respectively by $\frac{2}{r-l},\frac{2}{t-b},\frac{2}{f-n}$) so that all side-lengths of the cuboid become $2$. The overall transform is given by the matrix
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
0&0&\color{blue}{\frac{n+f}{n-f}}&\color{blue}\frac{2nf}{n-f}
\\
0&0&{1}&0
\end{array}\right]\label {proj}
$$
If we assume face $N$ to be symmetric about the $z$ axis, i.e., $l+r=t+b=0$, then Eq. $(\ref{proj})$ simplifies to
$$
^{\text{sym}}\b P = \b A\b S
=\left[\begin{array}{ccc}
\frac{-n}{r}&0&0&0
\\
0&\frac{-n}{t}&0&0
\\
0&0&{\color{blue}\frac{n+f}{n-f}}&\color{blue}\frac{2nf}{n-f}
\\
0&0&{1}&0
\end{array}\right]\label{sym}
$$

The coordinates system after applying the model, camera, and projection transformations is called the *normalized device coordinates* (NDC).

*Note*: In some graphics API's like OpenGL, the NDC system is defined to be left-handed (camera pointing to $\hat{\b z}$ instead of $-\hat{\b z}$; the $x$ and $y$ components unchanged) so that a larger $z$-value in NDC corresponds to greater depth from the camera. To get this type of projection matrix, simply negate the blue elements in $(\ref {proj})$ or $(\ref{sym})$.

#### Effects of projection on depth

Let's examine matrix $\b P$'s effects on the $z$-value of the points between the near/far planes:
$$
z'=
\frac1z\left[\begin{array}{c} \frac{n+f}{n-f}&\frac{2nf}{n-f} \end{array}\right]\left[\begin{array}{c} z\\1 \end{array}\right]=\frac{1}{n-f}\left(\frac{2nf}{z} + n+f\right)
$$
<img src="assets/02_3D Transformations II/ndc_z.png" style="zoom: 33%;" />

Observe from the graph that all cross sections of the view frustum, except for the near and far planes, get pushed further away from the camera by projection transformation than they would have by scaling only. Also observe that as $z\to-f$, the "squishification" effect of the projection becomes increasingly severe. In fact, imagine letting $f$ approach $\infty$. Then the graph above becomes essentially a horizontal straight line to the left in the limit. The limiting transformation matrix $^{\infty}\b P$ can be constructed by finding the limit of $(\ref{proj})$ as $f\to \infty$:
$$
\begin{alignedat}{2}
\b P_{3,3}&=\frac{n+f}{n-f}\to\ \ {-1} & = {^{\infty}\b P_{3,3}}\\
\b P_{3,4}&=\frac{2nf}{n-f}\to-2n &\ = {^{\infty}\b P_{3,4}}

\end{alignedat}\implies 

^{\infty}\b P
=\left[\begin{array}{ccc}
\frac{-2n}{r-l}&0&\frac{l+r}{l-r}&0
\\
0&\frac{-2n}{t-b}&\frac{b+t}{b-t}&0
\\
0&0&{-1}&-2n
\\
0&0&{1}&0
\end{array}\right]
$$
with
$$
z'=\frac{-z-2n}{z}=\frac{-2n}z-1
$$
This projection transformation maps an infinitely long and deep frustum starting from the near plane to the canonical cube $K$.



### References

[1] https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html

[2] http://www.songho.ca/opengl/gl_projectionmatrix.html

---

**Next:** Rasterization (I)

*To be continued.*



