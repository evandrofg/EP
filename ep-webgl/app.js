/* 
 * MAC 420/5744 – Introdução à Computação Gráfica
 * Exercício-Programa 1
 * 
 * Evandro Fernandes Giovanini
 * Marcello Souza
 */

var gl;
var canvas;
var app = {};
app.meshes = {};
app.models = {};

var u = 0.125; // escala usada para diminuir tamanho dos modelos
var zoom = 1.0;   // zoom de visualização

var mMatrix = mat4.create();
var pMatrix = mat4.create();
var vMatrix = mat4.create();

var ort = false;

var color;

var corA = [0.9, 0.9, 0.9];
var corB = [0.0, 0.0, 0.0];
var corC = [0.0, 0.0, 0.5]; // "pretas" (azul escuro)
var corD = [1.0, 0.4, 0.4]; // "brancas" (verm claro)

function resizeCanvas() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);

}

function procShaders() {

    program = initShaders (gl, "vertex-shader", "fragment-shader");
    gl.useProgram (program);

    program.vertexPositionAttribute = gl.getAttribLocation(program, "aVertexPosition");
    gl.enableVertexAttribArray(program.vertexPositionAttribute);

    program.vertexNormalAttribute = gl.getAttribLocation(program, "aVertexNormal");
    gl.enableVertexAttribArray(program.vertexNormalAttribute);

    program.pMatrixUniform = gl.getUniformLocation(program, "uPMatrix");
    program.vMatrixUniform = gl.getUniformLocation(program, "uVMatrix");
    program.mMatrixUniform = gl.getUniformLocation(program, "uMMatrix");

    color = gl.getUniformLocation(program, "aColor");

}

function initBuffers() {
    for( mesh in app.meshes ){
        OBJ.initMeshBuffers( gl, app.meshes[ mesh ] );
        app.models[ mesh ] = {};
        app.models[ mesh ].mesh = app.meshes[ mesh ];
    }
}

function setMatrixUniforms()
{
    gl.uniformMatrix4fv(program.pMatrixUniform, false, pMatrix);
    gl.uniformMatrix4fv(program.vMatrixUniform, false, vMatrix);
    gl.uniformMatrix4fv(program.mMatrixUniform, false, mMatrix);
}

function drawObject( model, translacao, cor ){

    if (cor == 0) /* parte do tabuleiro */
        gl.uniform4f(color, corA[0], corA[1], corA[2], 1);
    else if (cor == 1) /* parte do tabuleiro */
        gl.uniform4f(color, corB[0], corB[1], corB[2], 1);
    else if (cor == 2) /* peça escura */
        gl.uniform4f(color, corC[0], corC[1], corC[2], 1);
    else if (cor == 3) /* peça branca */
        gl.uniform4f(color, corD[0], corD[1], corD[2], 1);
    mat4.identity(mMatrix);

    /* centraliza o modelo no tabuleiro */
    mat4.translate(mMatrix, [translacao[0]-0.5+u/2.0, translacao[1], translacao[2]-0.5+u/2.0 ]);

    /* corrige a escala */
    mat4.scale(mMatrix, [u, u, u]);

    /* se é uma parte do tabuleiro (cor 0 ou 1), deixa mais achatado */
    if (cor <= 1){
        mat4.scale(mMatrix, [1.0, 1.0/2.0, 1.0]);
        mat4.translate(mMatrix, [0, 4.0*u, 0]);
    }

    gl.bindBuffer(gl.ARRAY_BUFFER, model.mesh.vertexBuffer);
    gl.vertexAttribPointer(program.vertexPositionAttribute, model.mesh.vertexBuffer.itemSize, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ARRAY_BUFFER, model.mesh.normalBuffer);
    gl.vertexAttribPointer(program.vertexNormalAttribute, model.mesh.normalBuffer.itemSize, gl.FLOAT, false, 0, 0);


    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, model.mesh.indexBuffer);

    setMatrixUniforms();
    gl.drawElements(gl.TRIANGLE_STRIP, model.mesh.indexBuffer.numItems, gl.UNSIGNED_SHORT, 0);
}

function degToRad(degrees) {
    return degrees * Math.PI / 180;
}

function webGLStart(meshes){
    app.meshes = meshes;
    
    canvas = document.getElementById("canvas");
    gl = canvas.getContext("webgl");
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;
    
    procShaders();
    initBuffers();

    gl.clearColor(1.0, 1.0, 1.0, 1.0);
    gl.enable(gl.DEPTH_TEST);

    resizeCanvas();

    document.getElementById("orthoTrueButton").onclick = function(){ort=true; mat4.identity(quartRotationMatrix);};
    document.getElementById("orthoFalseButton").onclick = function(){ort=false; mat4.identity(quartRotationMatrix);};
    window.addEventListener('resize', resizeCanvas);

    canvas.onmousedown = handleMouseDown;
    document.onmouseup = handleMouseUp;
    document.onmousemove = handleMouseMove;
    tick();

}

function tick()
{
    requestAnimFrame(tick);
    render();
}

function render()
{
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    ratio = gl.viewportWidth / gl.viewportHeight;

    if (ort == true){
        mat4.identity(vMatrix);
        mat4.ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0, pMatrix);
        /* modelar para o aspect ratio da tela */
        mat4.scale(vMatrix, [1.0/ratio, 1.0, 1.0/ratio]);
        mat4.multiply(vMatrix, quartRotationMatrix);

        /* zoom */
        mat4.scale(vMatrix, [zoom, zoom, zoom]);

    }

    if (ort == false){
        mat4.perspective(45, ratio, 0.1, 100.0, pMatrix);

        var eye = [1.0, 1.0, 2.0];
        var at = [0.0, 0.0, 0.0];
        var up = [0.0, 1.0, 0.0];

        mat4.lookAt(eye, at, up, vMatrix);
        mat4.multiply(vMatrix, quartRotationMatrix);
        mat4.scale(vMatrix, [zoom, zoom, zoom]);
    }

    // tabuleiro
    for (i = 0; i < 8; i++){
        drawObject( app.models.cubo, [ i*u, -u/2.0, 0*u], i % 2);
        drawObject( app.models.cubo, [ i*u, -u/2.0, 1*u], (i+1) % 2);
        drawObject( app.models.cubo, [ i*u, -u/2.0, 2*u], i % 2);
        drawObject( app.models.cubo, [ i*u, -u/2.0, 3*u], (i+1) % 2);
        drawObject( app.models.cubo, [ i*u, -u/2.0, 4*u], i % 2);
        drawObject( app.models.cubo, [ i*u, -u/2.0, 5*u], (i+1) % 2);
        drawObject( app.models.cubo, [ i*u, -u/2.0, 6*u], i % 2);
        drawObject( app.models.cubo, [ i*u, -u/2.0, 7*u], (i+1) % 2);
    }

    // peças
    for (i = 0; i < 8; i++)
        drawObject (app.models.peao, [i*u, 0.0, 1.5*u], 2);
    drawObject( app.models.torre,   [0*u, 0.0, 0.5*u], 2 );
    drawObject( app.models.cavalo,  [1*u, 0.0, 0.5*u], 2 );
    drawObject( app.models.bispo,   [2*u, 0.0, 0.5*u], 2 );
    drawObject( app.models.rainha,  [3*u, 0.0, 0.5*u], 2 );
    drawObject( app.models.rei,     [4*u, 0.0, 0.5*u], 2 );
    drawObject( app.models.bispo,   [5*u, 0.0, 0.5*u], 2 );
    drawObject( app.models.cavalo,  [6*u, 0.0, 0.5*u], 2 );
    drawObject( app.models.torre,   [7*u, 0.0, 0.5*u], 2 );

    for (i = 0; i < 8; i++)
        drawObject (app.models.peao, [i*u, 0.0, 6.5*u], 3);
    drawObject( app.models.torre,   [0*u, 0.0, 7.5*u], 3 );
    drawObject( app.models.cavalo,  [1*u, 0.0, 7.5*u], 3 );
    drawObject( app.models.bispo,   [2*u, 0.0, 7.5*u], 3 );
    drawObject( app.models.rainha,  [3*u, 0.0, 7.5*u], 3 );
    drawObject( app.models.rei,     [4*u, 0.0, 7.5*u], 3 );
    drawObject( app.models.bispo,   [5*u, 0.0, 7.5*u], 3 );
    drawObject( app.models.cavalo,  [6*u, 0.0, 7.5*u], 3 );
    drawObject( app.models.torre,   [7*u, 0.0, 7.5*u], 3 );
}


function trackballVec(x, y)
{
    width = canvas.width;
    height = canvas.height;

    var v = vec3.create();

    v[0] = (2.0*x - width) / width;
    v[1] = (height - 2.0*y) / height;

    d = Math.sqrt(v[0]*v[0] + v[1]*v[1]);
    if(d < 1.0) v[2] = Math.cos((Math.PI/2.0)*d);
      else v[2] = 0.0;

    a = 1.0 / Math.sqrt(vec3.dot(v,v));
    v = vec3.scale(v, a);

    return v;
}

var mouseDown = false;

function handleMouseDown(event) {
    mouseDown = true;
    lastMouseX = event.clientX;
    lastMouseY = event.clientY;
}

function handleMouseUp(event) {
    mouseDown = false;

}

var lastMouseX = null;
var lastMouseY = null;
var lastMouse2X = null;
var lastMouse2Y = null;

function handleMouseMove(event){
    if (event.button == 0)
        handleMouseMove0(event);
    else if (event.button == 2 || event.button == 1)
        handleMouseMove2(event);
}

function handleMouseMove2(event){
    if (!mouseDown)
        return;

    var newX = event.clientX;
    var newY = event.clientY;

    if (newY < lastMouse2Y)
        if (zoom < 2.0){
            zoom = zoom + 0.05;
        }
    if (newY > lastMouse2Y)
        if (zoom > 0.5){
            zoom = zoom - 0.05;
        }

    lastMouse2X = newX
    lastMouse2Y = newY;
}

var axis = vec3.create();
var rquat = quat4.create();
rquat = [1.0, 0.0, 0.0, 0.0];

var quartRotationMatrix = mat4.create();
mat4.identity(quartRotationMatrix);

function handleMouseMove0(event) {
    if (!mouseDown) {
        return;
    }
    var newX = event.clientX;
    var newY = event.clientY;

    var curPos = vec3.create();
    curPos = trackballVec(newX, newY);

    if (lastMouseX == null)
        return;
    var lastPos = vec3.create();
    lastPos = trackballVec(lastMouseX, lastMouseY);

    var d = vec3.create();
    d = vec3.subtract(curPos, lastPos);

    var a = vec3.dot(d, d);

    if (a){
        angle = (Math.PI/2.0) * Math.sqrt(a);
        axis = vec3.cross (lastPos, curPos);
        a = 1.0/Math.sqrt(vec3.dot(axis, axis));

        rnew = quat4.fromAngleAxis(angle,vec3.normalize(axis));

        rquat = quat4.multiply( rnew, rquat);
        quat4.toMat4 (rquat, quartRotationMatrix);
    }

    lastMouseX = newX
    lastMouseY = newY;
}

window.onload = function(){
    OBJ.downloadMeshes({
    'bispo': 'models/bispo.obj',
    'cavalo': 'models/cavalo.obj',
    'peao': 'models/peao.obj',
    'rainha': 'models/rainha.obj',
    'rei': 'models/rei.obj',
    'torre': 'models/torre.obj',
    'cubo': 'models/cubo.obj',
}, webGLStart);
}
