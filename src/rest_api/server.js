const express = require('express');
const db = require('./config/keys').mongoURI;


const app = express();

app.use(express.json({ limit: "100mB"}));
app.use(express.urlencoded({ extended: true }));
app.use('/api/user', require('./routes/api/user'));
app.use('/api/class', require('./routes/api/class'));
app.use('/api/model', require('./routes/api/model'));

// Start the server on a specified port or 5000
const port = process.env.PORT || 5000;
app.listen(port, () => {
    console.log(`Server started on port ${port}`);
});
