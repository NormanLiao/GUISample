#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vTangent;
out vec3 vBinormal;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
	vNormal = normalize( vec4(VertexNormal, 0.0).xyz );
	vTangent = normalize( vec4(VertexTangent.xyz, 0.0).xyz );
	vBinormal = normalize( cross( vNormal, vTangent ) * VertexTangent.w );
	vTexCoord = VertexTexCoord;
	vPosition = VertexPosition;
	
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4( VertexPosition, 1.0 );
}



