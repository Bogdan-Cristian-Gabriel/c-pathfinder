const express = require('express');
const cors = require('cors');
const { execFile } = require('child_process');

const app = express();
const port = 3000;

// The page is opened from a file:// or a different origin than the API
app.use(cors());

// Serve index.html and the rest of the project files as static assets
app.use(express.static(__dirname));

app.get('/api/route', (req, res) => {
	const { startLat, startLon, destLat, destLon } = req.query;
	if (!startLat || !startLon || !destLat || !destLon) {
		return res.status(400).json({ error: "Missing coordinates!"});
	}
	// Shell out to the C routing engine; the argument order is
	// startLat startLon destLat destLon, matching its argv
	execFile('./pathfinder', [startLat, startLon, destLat, destLon], (error, stdout, stderr) => {
		if (error) {
			return res.status(500).json({error: "Routing engine failed"});
		}
		try {
			// The engine prints a GeoJSON coordinate array on stdout
			const route = JSON.parse(stdout);
			res.json(route);
		} catch (parseError) {
			console.error("JSON parsing error:", parseError);
			res.status(500).json({ error: "Invalid output"});
		}
	});
});

app.listen(port, () => { console.log("Server started!"); });
