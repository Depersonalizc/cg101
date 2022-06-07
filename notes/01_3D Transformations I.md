## 3D Transformations (I)

Computer graphics deals primarily with transformations happening in $\R^3$. Most notably, the families of *linear*, *affine*, and *projective* transformations. $\newcommand {\b}{\mathbf} \newcommand{\para}{\mathbin{\!/\mkern-5mu/\!}}$

### Linear transformations in $\R^3$

Any linear transformation $\mathcal T:\R^3\to \R^3$ can be uniquely represented as a 3x3 real matrix $\b M$ whose columns can be found by tracking the canonical basis vectors after the transformation:
$$
\mathbf M=
\left[\begin{array}{}
\mathcal T(\hat{\b{x}})&\mathcal T(\hat{\b{y}}) &\mathcal T(\hat{\b{z}})
\end{array}\right]
$$
for which
$$
\mathcal T(\b v)=\b M\b v\quad\forall \b v\in\R^3
$$

#### Important linear transforms

- ***Change of basis***
  $$
  \b v'=\b M\b v=\left[\begin{array}{}
  \b i&\b j &\b k
  \end{array}\right]\b v
  $$
  A linear transformation that takes the canonical basis $\hat{\b x},\hat{\b y},\hat{\b z}$ to vectors $\b i,\b j,\b k$, respectively.

  This transformation can also be thought of as a change of basis from the $\b i,\b j,\b k$ coordinate system to the $\hat{\b x},\hat{\b y},\hat{\b z}$ system.

  Conversely, to express a (canonical-basis) vector in the $\b i,\b j,\b k$ system, one applies the inverse transform represented by the inverse matrix $\b M^{-1}=\left[\begin{array}{}
  \b i&\b j &\b k
  \end{array}\right]^{-1}$.

- ***Scaling***

  - **Along the $x,y,z$ axes, each by $\b s=s_x,s_y,s_z$**

  $$
  \mathbf v'=\b M\b v=\left[\begin{array}{c}
  s_x\cdot v_x\\
  s_y\cdot v_y\\
  s_z\cdot v_z
  \end{array}\right]=\text{diag}(\mathbf s)\cdot\b v\label{scale}
  $$

  - **Along three (linearly independent) axes $\b i,\b j,\b k$, each by $\b s = s_i,s_j,s_k$**

    This can be achieved by

    1. linearly transforming $\b i,\b j,\b k$ to the canonical basis vectors by $\b B^{-1}= \left[\begin{array}{}
       \b i&\b j &\b k
       \end{array}\right]^{-1}$

       (Equivalently, a change of basis from the canonical to $\b i,\b j,\b k$)

    2. applying the canonical transform $(\ref{scale})$ above

    3. linearly transforming the canonical basis vectors back to $\b i,\b j,\b k$ by $\b B=\left[\begin{array}{}
       \b i&\b j &\b k
       \end{array}\right]$.

       (Equivalently, a change of basis from $\b i,\b j,\b k$ to the canonical)

    Combining the steps:
    $$
    \mathbf v'=\b M\b v=(\b B \cdot\text{diag}(\b s)\cdot \b B^{-1})\b v
    $$
    We will see this general change-of-basis technique being frequently used.

  - Finally, any negative scale factor yields a ***reflection***.

- ***Rotation***

  - **About the $x,y,$ or $z$ axis by $\theta$**

    - $x$ axis: Keep $\hat{\b x}$ still and rotate the $y$-$z$ plane.

    $$
    \mathbf v'=\b R_{x}(\theta)\b v=\left[\begin{array}{c}
    1&0&0\\
    0&\cos\theta&-\sin\theta\\
    0&\sin\theta&\cos\theta
    \end{array}\right]\b v\label{xrot}
    $$

    - $y$ axis: Keep $\hat{\b y}$ still and rotate the $z$-$x$ (note the order) plane.
      $$
      \mathbf v'=\b R_{y}(\theta)\b v=\left[\begin{array}{c}
      \cos\theta&0&\sin\theta\\
      0&1&0\\
      -\sin\theta&0&\cos\theta
      \end{array}\right]\b v\label{yrot}
      $$
      We see $-\theta$ in place of the expected $\theta$ as a consequence of the right-handedness of the basis $(\hat{\b x}\times\hat{\b z}=-\hat{\b y}$ instead of $\hat{\b y})$.

    - $z$ axis: Keep $\hat{\b z}$ still and rotate the $x$-$y$ plane.

    $$
    \mathbf v'=\b R_{z}(\theta)\b v=\left[\begin{array}{c}
    \cos\theta&-\sin\theta&0\\
    \sin\theta&\cos\theta&0\\
    0&0&1
    \end{array}\right]\b v\label{zrot}
    $$

  - **About an arbitrary axis $\hat{\b n}$ by $\theta$ (Rodrigues' Formula)**

    <img src="assets/01_3D Transformations I/rodrigues.png" alt="image-20220605021819086" style="zoom: 40%;" /> ***Fig***: Rotation on $\b v$ about (unit) vector $\hat{\b n}$

    A rotation on $\b v$ about an arbitrary axis $\hat{\b n}$ can be decomposed into two steps:

    1. Express $\b v$ as a combination of parallel and perpendicular components of $\hat{\b n}$

       (Effectively writing $\mathbf v$ in a new orthonormal $\hat{\b n},\hat{\b p},\hat{\b q}$ basis)

    2. Rotate the perpendicular component while keeping the parallel component unchanged

       (Rotation about the $\hat{\b n}$ axis, can be expressed neatly as a matrix)

    First, compute components of $\b v$:
    $$
    \begin{aligned}
    \b v_\para &= \hat{\b n}\cdot \hat{\b n}^\top \b v\\
    \b v_\perp &= \b v-\b v_\para = (\b I-\hat{\b n} \hat{\b n}^\top)\b v
    \end{aligned}
    $$
    Take the basis vectors
    $$
    \hat{\b p}=\frac{\b v_\perp}{\|\b v_\perp\|}\\
    \hat{\b q}=\hat{\b n}\times \hat{\b p}=\frac{\hat{\b n}\times\b v}{\|\b v_\perp\|}
    $$
    so that
    $$
    \b v = \left[\begin{array}{}
    \hat{\b n}&\hat{\b p} &\hat{\b q}
    \end{array}\right]
    \left[\begin{array}{c}
    \|\b v_\para\| \\ \|\b v_\perp\| \\ 0
    \end{array}\right]
    $$
    Rotating about the $\hat{\b n}$ axis then becomes easy:
    $$
    \begin{aligned}
    \b v'
    &=
    
    \left[\begin{array}{c}
    \hat{\b n}&\hat{\b p}&\hat{\b q}
    \end{array}\right]
    
    \left[\begin{array}{c}
    1&0&0\\
    0&\cos\theta&-\sin\theta\\
    0&\sin\theta&\cos\theta
    \end{array}\right]
    
    \left[\begin{array}{c}
    \|\b v_\para\| \\ \|\b v_\perp\| \\0
    \end{array}\right]
    
    \\&=\|\b v_\para\|\hat{\b n}+\|\b v_\perp\|(\cos\theta\cdot\hat{\b p}+\sin\theta\cdot\hat{\b q})
    
    \\&=\b v_\para+\cos\theta \cdot\b v_\perp+\sin\theta\cdot(\hat{\b n}\times\b v)
    \\&=\hat{\b n}\hat{\b n}^\top\b v+\cos\theta\cdot(\b I-\hat{\b n}\hat{\b n}^\top)\b v+\sin\theta\cdot\b N\b v\\
    &=[(1-\cos\theta)\hat{\b n}\hat{\b n}^\top+\cos\theta\cdot\b I+\sin\theta\cdot\b N]\b v
    
    \end{aligned}
    $$
    where
    $$
    \b N=\left[\begin{array}{c}
    0&-\hat n_z&\hat n_y\\
    \hat n_z&0&-\hat n_x\\
    -\hat n_y&\hat n_x&0
    \end{array}\right]
    $$
    This is **Rodrigues' Formula**.

  - **As Euler angles**

    Euler's rotation theorem says any rotation can be expressed as three consecutive axis-aligned sub-rotations. For example, the figure below illustrates a $z$-$x'$-$z'$ rotation with Euler angles $(\phi,\theta,\psi)$.

    <img src="https://mathworld.wolfram.com/images/eps-svg/EulerAngles_600.svg" alt="EulerAngles" style="zoom:80%;" />

    We shall next derive an expression of the rotation matrix $\b M=\b R_{BCD}$ in terms of $(\phi,\theta,\psi)$. 

    The first rotation $D$ is given by Eq. $(\ref{zrot})$
    $$
    \mathbf R_D=\b R_{z}(\phi)
    $$
    To apply a second rotation $C$ on top we resort to the change-of-basis technique: First undo $D$, then apply rotation about the $x$-axis, and finally apply $D$ again:
    $$
    \b R_{CD}=\b R_D\b R_{x}(\theta)\b R_D^{-1}\b R_D=\b R_{z}(\phi)\b R_{x}(\theta)
    $$
    Adding the final rotation $B$ is similar: Undo the previous rotations, apply the rotation about the $z$-axis, and bring the previous rotations back in the end:
    $$
    \b R_{BCD}=\b R_{CD}\b R_{z}(\psi)\b R_{CD}^{-1}\b R_{CD}=\b R_{z}(\phi)\b R_{x}(\theta)\b R_{z}(\psi)\\
    =\left[\begin{array}{c}
    \cos\phi\cos\psi-\sin\phi \cos\theta\sin\psi & -\cos\phi\sin\psi-\sin\phi\cos\theta\cos\psi & \sin\phi\sin\theta\\
    \sin\phi\cos\psi+\cos\phi\cos\theta\sin\psi & -\sin\phi\sin\psi+\cos\phi \cos\theta\cos\psi & -\cos\phi\sin\theta\\
    \sin\theta\sin\psi& \sin\theta\cos\psi & \cos\theta
    \end{array}\right]
    $$
    Note the reversed order of matrix multiplications.

### Affine transformations in $\R^3$

#### Homogeneous coordinates

We associate each point $(x,y,z)\in\R^3$ with a collection of points in the homogeneous (projective) space $\mathbb P^3$:
$$
\R^3 \ni (x,y,z)\quad\sim\quad(xw,yw,zw,w)\in \mathbb P^3,\qquad w\neq 0
$$
Conversely, each point $(x,y,z,w),w\neq 0$ in the projective space $\mathbb P^3$ represents the 3D Cartesian *point* $(x/w,y/w,z/w)$:
$$
\mathbb P^3 \ni (x,y,z,w)\quad\sim\quad(x/w,y/w,z/w)\in \R ^3,\qquad w\neq 0
$$
In particular, $w=1$ gives the "canonical" homogeneous coordinates $(x,y,z,1)$ of a 3D point $(x,y,z)$.

For graphics purposes it is convenient to define $(x,y,z,0)\in \mathbb P^3$ as representing the 3D Cartesian *vector* $(x,y,z)$. This definition has the desirable properties that points in $\mathbb P^3 $ representing

- Vector + Vector = Vector
- Point + Vector (or Vector + Point) = Point

in $\R^3$.

Homogeneous coordinates make it possible to write any affine transformation $\mathcal A:\R^3\to\R^3$ 
$$
\mathcal A(\b v)=\mathcal T(\b v)+\b t=\b M\b v+\b t
$$
as a linear transformation in $\mathbb P^3$. Such a transformation can be uniquely represented as a 4x4 real matrix
$$
\b F=\left[\begin{array}{c|c}
\b M& \b t\\

\b 0^\top& 1

\end{array}\right]
$$
that acts on the canonical homogeneous coordinates $\tilde{\b v}=[\b v;1]$:
$$
\b F \tilde{\b v}=\left[\begin{array}{c}
\b M \b v+\b t\\
 1

\end{array}\right]\sim \mathcal A(\b v)
$$

#### Important affine transforms

- ***Pure translation***
  $$
  \tilde{\b v}'=\b F\tilde{\b v}=\left[\begin{array}{c}
  v_x+t_x\\
  v_y+t_y\\
  v_z+t_z\\
  1
  \end{array}\right]=\left[\begin{array}{c|c}
  \b I& \b t\\
  \b 0^\top& 1
  
  \end{array}\right]\tilde{\b v}
  $$

- ***Pure linear transform***
  $$
  \tilde{\b v}'=\b F\tilde{\b v}=\left[\begin{array}{c}
  \b M\b v\\
  1
  \end{array}\right]=\left[\begin{array}{c|c}
  \b M& \b 0\\
  \b 0^\top& 1
  
  \end{array}\right]\tilde{\b v}
  $$

- ***Inverse of an affine transform***

  Suppose we are given an affine transform $\b v\mapsto \mathbf M\b v+\b t$ represented by the matrix
  $$
  \b F=\left[\begin{array}{c|c}
  \b M& \b t\\
  \b 0^\top& 1
  
  \end{array}\right]
  $$
  The inverse matrix
  $$
  \b F^{-1}=\left[\begin{array}{c|c}
  \b M^{-1}& -\b M^{-1}\b t\\
  \b 0^\top& 1
  \end{array}\right]
  $$
  represents another affine transform
  $$
  \b v\mapsto \mathbf M^{-1}\b v-\b M^{-1}\b t=\b M^{-1}(\b v -\b t)
  $$
  Note the order: The inverse transform first undoes the translation, and then reverses the linear transform.

- ***Rotation about an axis of direction $\hat{\b n}$ centered at $\b c$***

  We already knew how to rotate about an axis centered at zero. To rotate around one that is not, simply apply the change-of-basis technique:

  1. Translate the center $\b c$ to $\b 0$ (Pure translation)
  2. Apply Rodrigues' Formula (Linear)
  3. Translate $\b 0$ back to $\b c$ (Pure translation)



### References

[1] https://mathworld.wolfram.com/EulerAngles.html

[2] https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html

[3] https://mathfor3dgameprogramming.com/

---

**Next:** Model-View-Projection; Projective transforms

*To be continued.*



