const express = require('express');
const db = require('./config/keys').mongoURI;

// Create the express app
const app = express();

// Use middleware and routes.
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use('/api/user', require('./routes/api/user'));
app.use('/api/class', require('./routes/api/class'));

// Start the server on a specified port or 5000
const port = process.env.PORT || 5000;
app.listen(port, () => {
    console.log(`Server started on port ${port}`);
});
