async function main() {
  const adapter = await navigator.gpu?.requestAdapter();
  const device = await adapter?.requestDevice();
  if (!device) {
    fail('need a browser that supports WebGPU');
    return;
  }

  const nbIntegers = 8;
  //const nbIntegers = 2 << 4;
  const inputs = createSuccessiveArray(nbIntegers);
  printArray("inputs", inputs, nbIntegers);

  const dispatchCount = [1, 1, 1];
  const workgroupSize = [nbIntegers, 1, 1];
 
  const code = `
  @group(0) @binding(0) var<storage, read_write> inputResult: array<u32>;
  @group(0) @binding(1) var<storage, read_write> outputResult: array<u32>;

  @compute @workgroup_size(${workgroupSize}) fn computeSomething(
      @builtin(global_invocation_id) global_invocation_id : vec3<u32>
  ) {
    outputResult[global_invocation_id.x] = inputResult[global_invocation_id.x] * 2;
  }
  `;

  const module = device.createShaderModule({code});

  const pipeline = device.createComputePipeline({
    label: 'compute pipeline',
    layout: 'auto',
    compute: {
      module,
    },
  });

  const bufferSizeInBytes = nbIntegers * 4; // n integers * 4 bytes

  let usage = GPUBufferUsage.STORAGE | GPUBufferUsage.COPY_DST;
  const inputBuffer = device.createBuffer({size: bufferSizeInBytes, usage});

  // Copy data from CPU to GPU - WebGPU choose the most efficient way to getting data to our buffer
  device.queue.writeBuffer(inputBuffer, 0, inputs);

  usage = GPUBufferUsage.STORAGE | GPUBufferUsage.COPY_SRC;
  const outputBuffer = device.createBuffer({size: bufferSizeInBytes, usage});

  usage = GPUBufferUsage.MAP_READ | GPUBufferUsage.COPY_DST;
  const outputReadBuffer = device.createBuffer({size: bufferSizeInBytes, usage});

  const bindGroup = device.createBindGroup({
    layout: pipeline.getBindGroupLayout(0),
    entries: [
      { binding: 0, resource: { buffer: inputBuffer }},
      { binding: 1, resource: { buffer: outputBuffer }},
    ],
  });

  // Encode commands to do the computation
  const encoder = device.createCommandEncoder({ label: 'compute builtin encoder' });
  const pass = encoder.beginComputePass({ label: 'compute builtin pass' });

  pass.setPipeline(pipeline);
  pass.setBindGroup(0, bindGroup);
  pass.dispatchWorkgroups(...dispatchCount);
  pass.end();

  encoder.copyBufferToBuffer(outputBuffer, 0, outputReadBuffer, 0, bufferSizeInBytes);

  // Finish encoding and submit the commands
  const commandBuffer = encoder.finish();
  device.queue.submit([commandBuffer]);

  // Read the results back to CPU
  await Promise.all([
    outputReadBuffer.mapAsync(GPUMapMode.READ),
  ]);
  const outputs = new Uint32Array(outputReadBuffer.getMappedRange());
  printArray("outputs", outputs, nbIntegers);
}

function createSuccessiveArray(size) {
  var arr = new Uint32Array(size);
  for (let i = 0; i < size; ++i) {
    arr[i] = i + 1;
  }
  return arr;
}

function printArray(title, arr, size) {
  var str = title + ": ";
  for (let i = 0; i < size; ++i) {
    str += arr[i];
    str += ",";
    // Jump to n - 4
    if (i == 3 && size > 8) {
      str += "...,";
      i = size - 5;
    }
  }
  log(str);
}

function log(...args) {
  const elem = document.createElement('pre');
  elem.textContent = args.join(' ');
  document.body.appendChild(elem);
}

function fail(msg) {
  // eslint-disable-next-line no-alert
  alert(msg);
}

