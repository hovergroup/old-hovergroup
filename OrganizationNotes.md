# Introduction #

Use this page as a reference for organization of the svn.


# Details #

**Trunk**

Trunk contains current development versions of common libraries and programs.  Any apps used for general operation of the kayaks, nodes, or shoreside should exist in the trunk.  Do not leave commits in the trunk that will cause build errors.

**Kayak-stable**

Branch of the trunk containing the subset of apps and libraries required for operation of the kayaks.  Updates in the trunk should be merged into this branch whenever possible.  Meant to reduce build times on the vehicles.

**Individual Folders**

Your own apps and programs should reside in individual folders.  When running experiments using your own apps, modify the system path to include your /bin folder.  (This will later be done in launch scrips.)