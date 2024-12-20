## Convey

get example

```
  wServer.get("/get", [](Request &req, Response &res) {
    json jsonResponse;
    jsonResponse["Stream"] = "192.168.1.1";
    jsonResponse["Port"] = std::to_string(PORT);
    res.json(jsonResponse.dump(4));
  });
```

