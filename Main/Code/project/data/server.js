// Minimal Express server to accept POST /sendData for testing the forms
const express = require('express');
const path = require('path');
const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Serve static files (the index.html and assets)
app.use('/', express.static(path.join(__dirname)));

app.post('/sendData', (req, res) => {
  console.log('Received /sendData POST:', new Date().toISOString());
  console.log('Body:', JSON.stringify(req.body, null, 2));
  // simple validation example
  if (!req.body || !req.body.formId) {
    return res.status(400).send('Missing formId');
  }
  // Respond with JSON
  res.json({ status: 'ok', received: req.body });
});

app.listen(PORT, () => {
  console.log(`Test server running at http://localhost:${PORT}/`);
});
