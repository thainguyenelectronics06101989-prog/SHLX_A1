// get contests
let formContest = document.getElementsByClassName("contest")[0];

let button_refresh = formContest.getElementsByClassName("button")[0];
let button_send = formContest.getElementsByClassName("button")[1];
let button_run = formContest.getElementsByClassName("button")[2];
let button_stop = formContest.getElementsByClassName("button")[3];
button_refresh.addEventListener("click", async () => {
  button_refresh.disabled = true;
  try {
    const formData = new FormData(formContest); // lấy toàn bộ field có name=
    const payload = Object.fromEntries(formData.entries());
    console.log(JSON.stringify(payload));

    const resp = await fetch("/contest", {
      method: "GET",
      headers: { "Content-Type": "application/json" },
    });

    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    const data = await resp.json().catch(() => ({})); // nếu server trả JSON
    console.log("Data from server:", data);

    updateFormInputs(formContest, data);

    console.log("Response:", data);
  } catch (err) {
    console.error(err);
  } finally {
    button_refresh.disabled = false;
  }
});

button_send.addEventListener("click", async () => {
  button_send.disabled = true;
  try {
    const formData = new FormData(formContest); // lấy toàn bộ field có name=
    const payload = Object.fromEntries(formData.entries());
    console.log(JSON.stringify(payload));

    const resp = await fetch("/contest", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload),
    });

    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    const data = await resp.json().catch(() => ({})); // nếu server trả JSON
    console.log("Data from server:", data);

    updateFormInputs(formContest, data);

    console.log("Response:", data);
  } catch (err) {
    console.error(err);
  } finally {
    button_send.disabled = false;
  }
});
button_run.addEventListener("click", async () => {
  button_run.disabled = true;
  try {
    const resp = await fetch("/run", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: '{"run":true}',
    });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    const data = await resp.json().catch(() => ({}));
    console.log("Data from server:", data);
    console.log("Response:", data);
  } catch (err) {
    console.error(err);
  } finally {
    button_run.disabled = false;
  }
});
button_stop.addEventListener("click", async () => {
  button_stop.disabled = true;
  try {
    const resp = await fetch("/run", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: '{"run":false}',
    });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    const data = await resp.json().catch(() => ({}));
    console.log("Data from server:", data);
    console.log("Response:", data);
  } catch (err) {
    console.error(err);
  } finally {
    button_stop.disabled = false;
  }
});

function updateFormInputs(form, data) {
  Object.entries(data).forEach(([key, value]) => {
    const input = form.querySelector(`[name="${key}"]`);
    if (input) {
      input.value = value;
    }
  });
}
// get photo
let photo = document.getElementById("photo");
let button_image = document.getElementsByName("getImage")[0];
button_image.addEventListener("click", async () => {
  button_image.disabled = true;
  try {
    const resp = await fetch("/camera", {
      method: "GET",
      headers: { "Content-Type": "image/jpeg" },
    });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    const blob = await resp.blob();
    const imageUrl = URL.createObjectURL(blob);
    console.log("Blob:", blob);
    console.log("Image URL:", imageUrl);
    photo.src = imageUrl;
  } catch (err) {
    console.error(err);
  } finally {
    button_image.disabled = false;
  }
});

// cấu hình server
let formServer = document.getElementsByClassName("server")[0];

let button_refresh_server = formServer.getElementsByClassName("button")[0];
let button_send_server = formServer.getElementsByClassName("button")[1];
button_refresh_server.addEventListener("click", async () => {
  button_refresh_server.disabled = true;
  console.log("Refresh server config");
  try {
    const formData = new FormData(formServer); // lấy toàn bộ field có name=
    const payload = Object.fromEntries(formData.entries());
    console.log(JSON.stringify(payload));

    const resp = await fetch("/server", {
      method: "GET",
      headers: { "Content-Type": "application/json" },
    });

    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    const data = await resp.json().catch(() => ({})); // nếu server trả JSON
    console.log("Data from server:", data);

    updateFormInputs(formServer, data);

    console.log("Response:", data);
  } catch (err) {
    console.error(err);
  } finally {
    button_refresh_server.disabled = false;
  }
});
button_send_server.addEventListener("click", async () => {
  button_send_server.disabled = true;
  console.log("Send server config");
  try {
    const formData = new FormData(formServer); // lấy toàn bộ field có name=
    const payload = Object.fromEntries(formData.entries());
    console.log(JSON.stringify(payload));

    const resp = await fetch("/server", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload),
    });

    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    const data = await resp.json().catch(() => ({})); // nếu server trả JSON
    console.log("Data from server:", data);

    updateFormInputs(formServer, data);

    console.log("Response:", data);
  } catch (err) {
    console.error(err);
  } finally {
    button_send_server.disabled = false;
  }
});

// update OTA
var prg = document.getElementById("prg");
var form = document.getElementById("upload-form");
form.addEventListener("submit", (el) => {
  prg.style.backgroundColor = "blue";
  el.preventDefault();
  var data = new FormData(form);
  var req = new XMLHttpRequest();
  var fsize = document.getElementById("file").files[0].size;
  req.open("POST", "/update?size=" + fsize);
  req.upload.addEventListener("progress", (p) => {
    let w = Math.round((p.loaded / p.total) * 100) + "%";
    if (p.lengthComputable) {
      prg.innerHTML = w;
      prg.style.width = w;
    }
    if (w == "100%") prg.style.backgroundColor = "black";
  });
  req.send(data);
});
