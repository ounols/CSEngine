<?xml version="1.0" encoding="utf-8"?>
<CSESHADER version="1.0.0">
<shader pass="forward" v="PBR.vert" f="PBR.frag" localPath="1"/>
<shader pass="geometry" v="PBR-Geometry-Pass.vert" f="PBR-Geometry-Pass.frag" localPath="1"/>
<shader pass="deferred" v="PBR-Light-Pass.vert" f="PBR-Light-Pass.frag" localPath="1"/>
<shader pass="depthOnly" v="File:Shader/Shadow/default_shadow.vert" f="File:Shader/Shadow/default_shadow.frag"/>
</CSESHADER>