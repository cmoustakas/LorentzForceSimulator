# LorentzForceSimulator
Simulation of a particle's behaviour near a charged stick


Lorentz Force Simulator

Chares Moustakas

15/12/2022

1





Lorentz Force Simulator

1 Introduction

The current software is attempting to simulate the behaviour of a particle near

a ﬁnite long charged wire. It is important to underline that the aforementioned

software is just a fun, non-optimized project and as a consequence the reader

can feel free to use, modify, improve and play with it. It is notable to mention

that this software was developed in C++ using relatively low level APIs and

Libraries such as modern OpenGL, custom fragment-vertex shaders through

GLSL, Dear ImGUI and CMake, while all the Objects-meshes were constructed

through Blender.

2 Mathematics

The concept of this simulation is to calculate the Lorentz Force that the

wire exterts to the spherical particle, using primarily the current that ﬂows

through the wire as a datum. To begin with, it is crucial to visualize from the

Mathematical perspective the goal, being said that the formula for the precious

calculation is:

−→

→−

→−

→−

F

= q( E + u × B)

L

Where q is Coulomb charge of the particle, E the electric ﬁeld, u particle’s

velocity and B the magnetic’s ﬁeld ﬂux density.

2.1 B calculation

The calculation of B is based on Ampere’s formula:

I

→− →−

B dl = µI

C

Due to B and dl parallelism, the equation above is tranformed as:

I

−→

|B|

dl = µI

C

−→

|B|2πr = µI

−→

µI

|B| =

2πr

→−

B =

µI

ˆ

φ

2πr

Cylindrical Coordinates:

z

x

ˆ

φ = − xˆ + zˆ

r

r

Section 2

Chares Moustakas

2





Lorentz Force Simulator

It is notable that the direction of B is tangent to the closed circle-loop C.

Consequently B calculation is possible if the position of the particle and the

current are known magnitudes.

2.2 E calculation

Regarding Electric ﬁeld calculation, patience is the key... So lets construct

the main acceptance, ﬁrst of all lets assume that the wire has L length. An

inﬁnitesmal charged part of the wire dq that is y units away from the end of

the cable causes Electric ﬁeld dE for a random point P that it is r units away

from the wire:

Figure 1: Calculation of Electric ﬁeld

dq

dy

Q

L

\=

Q

dq = dy

L

−→

dE = dE sinθyˆ + dE cosθrˆ

(1)

(2)

y

r

y

r

sinθ = p

, cosθ = p

Chares Moustakas

y2 + r2

y2 + r2

Section 2

3





Lorentz Force Simulator

dq

y2 + r2

dE = k

Based on the above:

Q

y

dEy = k

p

dy

L

(y2 + r2)3

Z

Z

Ey

L/2

Q

L

y

Q

−1

1

dEy = k

p

dy = k (r

\+ r

)

(y2 + r2)3

L

L2

L2

0

−L/2

(r2 +

)

(r2 +

)

4

4

Ey = 0

The only non zero coordinate is the coordinate of r unit vector, based on the

(1) ! Lets calculate the r coordinate.

Q

L

r

dEr = k

p

dy

(y2 + r2)3

Z

Z

Er

L/2

Q

L

r

Q

L/2

L/2

dEr = k

p

dy = k ( r

\+

r

)

(y2 + r2)3

L

L2

L2

0

−L/2

r

(r2 +

)

r

(r2 +

)

4

4

Q

E = k r

r

L2

r

r2 +

4

Consequently, the expression (1) can be written as:

→−

Q

E = k r

rˆ

(3)

L2

r

r2 +

4

Cylindrical coordinates:

x

z

rˆ =

\+

r

r

It is crucial to keep in mind that the Coulomb charge Q is not constant in

time, therefore on each OpenGL iteration the expression (3), concerning the

Electric ﬁeld calculation, is taken into account:

dQ

I =

dt

Section 2

Chares Moustakas

4

