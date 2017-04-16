#!/bin/sh
curl -v http://localhost:5000/hash --http1.0 -X POST -d @test.json
