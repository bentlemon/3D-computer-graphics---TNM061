var container;
var camera, scene, renderer;
var mouseX = 0,
    mouseY = 0;
var windowHalfX = window.innerWidth / 2;
var windowHalfY = window.innerHeight / 2;

// Object3D ("Group") nodes and Mesh nodes
var light = new THREE.PointLight(0xffffff, 1, 100); 
var ambientLight = new THREE.AmbientLight(0x202020);

var sceneRoot = new THREE.Group();

var sunSpin = new THREE.Group();
var sunMesh;

var earthSpin = new THREE.Group();
var earthMesh;

var moonSpin = new THREE.Group();
var moonMesh;

var marsSpin = new THREE.Group();
var marsMesh;

var animation = true;

function onWindowResize() {
    windowHalfX = window.innerWidth / 2;
    windowHalfY = window.innerHeight / 2;
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
}

function onDocumentMouseMove(event) {
    // mouseX, mouseY are in the range [-1, 1]
    mouseX = (event.clientX - windowHalfX) / windowHalfX;
    mouseY = (event.clientY - windowHalfY) / windowHalfY;
}

function createSceneGraph() {
    
    scene = new THREE.Scene();

    // Top-level node
    scene.add(sceneRoot);

    // SUN branch
    sceneRoot.add(sunSpin);
    sunSpin.add(sunMesh);

    // EARTH branch
    sunMesh.add(earthSpin);
    earthSpin.add(earthMesh);
    earthSpin.position.set(15, 0.0, 0.0);
    earthMesh.rotation.z = 24.3;

    // MARS branch , barn till solen 
    sunMesh.add(marsSpin);
    marsSpin.add(marsMesh);
    marsMesh.position.set(25, 0.0, 0.0);// Sätter en position utifrån de lokala korrdinaterna 
  
    // Moon branch, lägger till månen som barn till solen (jorden)
    earthSpin.add(moonSpin);
    moonSpin.add(moonMesh);
    moonSpin.position.set(5, 0.0, 0.0);
    var light = new THREE.PointLight(0xffffff, 1, 100); 

    // ljus 
    //---------------------
    light.position.set(0, 0, 25)

    scene.add(light)
    scene.add(ambientLight);
    //---------------------
}

function init() {
    container = document.getElementById('container');

    camera = new THREE.PerspectiveCamera(38, window.innerWidth / window.innerHeight, 0.5, 200);
    camera.position.z = 80;
    
    var texloader = new THREE.TextureLoader();
    
    // Earth 
	var geometrySun = new THREE.SphereGeometry(4, 28, 100);  
    var geometryEarth = new THREE.SphereGeometry(2, 18, 100);
    var geometryMoon = new THREE.SphereGeometry(0.5, 50, 100);
    var geometryMars = new THREE.SphereGeometry(1.3 , 50, 100);


    var materialSun = new THREE.MeshLambertMaterial();
    materialSun.combine = 0;
    materialSun.needsUpdate = true;
    materialSun.wireframe = false;  
    
    // Jorden  variabler
    var materialEarth = new THREE.MeshBasicMaterial();
    materialEarth.combine = 0;
    materialEarth.needsUpdate = true;
    materialEarth.wireframe = false; 

    var materialMoon = new THREE.MeshBasicMaterial();
    materialMoon.combine = 0;
    materialMoon.needsUpdate = true;
    materialMoon.wireframe = false; 
    
    var materialMars = new THREE.MeshBasicMaterial();
    materialMars.combine = 0;
    materialMars.needsUpdate = true;
    materialMars.wireframe = false;

    //
    // Task 2: uncommenting the following two lines requires you to run this example with a (local) webserver - KLART
    // see https://threejs.org/docs/#manual/en/introduction/How-to-run-things-locally
    //
    
    // Jordens textur!
	const earthTexture = texloader.load('tex/2k_sun.jpg');
    materialSun.map = earthTexture;

    // Solens textur! 
    const sunTexture = texloader.load('tex/2k_earth_daymap.jpg');
    materialEarth.map = sunTexture;

    // Månens textur!
    const moonTexture = texloader.load('tex/2k_moon.jpg');
    materialMoon.map = moonTexture;

    // Mars textur!
    const marsTexture = texloader.load('tex/2k_mars.jpg');
    materialMars.map = marsTexture;

    // Task 8: material using custom Vertex Shader and Fragment Shader
    //----------------------------------------------
	var uniforms = THREE.UniformsUtils.merge( [
	    { 
	    	colorTexture : { value : new THREE.Texture() },
	    	specularMap : { value : new THREE.Texture() }
    	},
	    THREE.UniformsLib[ "lights" ]
	] );

	const shaderMaterial = new THREE.ShaderMaterial({
		uniforms : uniforms,
		vertexShader : document.getElementById('vertexShader').textContent.trim(),
		fragmentShader : document.getElementById('fragmentShader').textContent.trim(),
		lights : true
	});
	shaderMaterial.uniforms.colorTexture.value = earthTexture;
	
	const specularMap = texloader.load('tex/2k_earth_specular_map.jpg');
	shaderMaterial.uniforms.specularMap.value = specularMap;
	// ---------------------------------------------

    sunMesh = new THREE.Mesh(geometrySun, materialSun);
    earthMesh = new THREE.Mesh(geometryEarth, materialEarth, shaderMaterial);
    moonMesh = new THREE.Mesh(geometryMoon, materialMoon);
    marsMesh = new THREE.Mesh(geometryMars, materialMars);

    createSceneGraph();

    renderer = new THREE.WebGLRenderer();
    renderer.setClearColor(0x000000);
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);

    container.appendChild(renderer.domElement);

    document.addEventListener('mousemove', onDocumentMouseMove, false);
    window.addEventListener('resize', onWindowResize, false);

    var checkBoxAnim = document.getElementById('animation');
    animation = checkBoxAnim.checked;
    checkBoxAnim.addEventListener('change', (event) => {
    	animation = event.target.checked;
    });

	var checkBoxWireframe = document.getElementById('wireframe');
    //earthMesh.material.wireframe = checkBoxWireframe.checked;
    checkBoxWireframe.addEventListener('change', (event) => {
    	//earthMesh.material.wireframe = event.target.checked;
    });
}

function render() {
    // Set up the camera
    camera.position.x = mouseX * 10;
    camera.position.y = -mouseY * 10;
    camera.lookAt(scene.position);

    // Perform animations
    if (animation) {
       sunMesh.rotation.y += 0.0025;
       earthSpin.rotation.y += 0.00273; // ändrar roteringsfarten på måne 
       earthMesh.rotation.y += 0.0365; // ändrar jordens rotaion
      
       marsSpin.rotation.y += 0.001;
       marsMesh.rotation.y += 0.01;
    }

    // Render the scene
    renderer.render(scene, camera);
}

function animate() {
    requestAnimationFrame(animate); // Request to be called again for next frame
    render();
}

init(); // Set up the scene
animate(); // Enter an infinite loop
