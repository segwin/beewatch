# REST API

## Get climate data

### URI:
`GET /api/v1/data/climate`

### Parameters (query):
The `since` parameter lets us specify the time of the earliest sample to get as a Unix timestamp. For example,
in the example below we will only get samples on or after 1545097428 (2018-12-18 01:43:48 UTC). Any earlier
samples won't be fetched.

If left unspecified, all samples will be fetched.

```
GET /api/v1/data/climate?since=1545097428
```

### Response:
The response separates the samples and timestamps into two separate arrays, which makes it easier to plot
them afterwards. It reports these values for both temperature sensors in the design, i.e. inside and outside
the enclosure.

```json
{
  'interior': {
    'timestamps': [
        1545034582
        (...)
        1545097428
      ],
    'samples': [
        {
          'temperature': 2.5,
          'humidity': 62.5
        },
        
        (...)

        {
          'temperature': 4.0,
          'humidity': 75.0
        }
      ]
  },

  'exterior': {
    'timestamps': [
        (...)
      ],
    'samples': [
        (...)
      ]
  },
}
```


## Get device name

### URI:
`GET /api/v1/name`

### Response:
Current device name.

```json
{
  'name': 'current name here'
}
```


## Set device name

### URI:
`PUT /api/v1/name`

### Parameters:

*Headers:* `Content-Type: application/json`

*Body:*

```json
{
  'name': 'new name here'
}
```

### Response:
New device name as returned by application.

```json
{
  'name': 'new name here'
}
```


## Get application version

### URI:
`GET /api/v1/version`

### Response:
Application version with major, minor, patch & revision tags.

```json
{
  'version': 'beewatch v0.1.0 (21e91fe47344)',
  'major': '0',
  'minor': '1',
  'patch': '0',
  'revision': '21e91fe47344'
}
```

