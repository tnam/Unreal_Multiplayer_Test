## Project Features

### Conversion Machines
- Simple machines that ingest specific assets and convert them into a new asset type.
- Example: Input 2 triangles to output 1 square.

### Shape Library
- A collection of shape items, including spheres, cubes, rectangular boxes, cylinders, and more.
- Organized using data tables.

### Generic Machine Actor
- A static actor represented by a simple mesh in-game.
- Serves as the machine that performs asset conversions.

### Recipe System
- A data table of recipes defining how one or more shapes can be combined to form a new shape.
- Each machine has a unique set of recipes.

### Interactive Shape Actors
- Shape items spawned around the map that players can interact with by picking up, dropping, or pushing them.

### Machine Operation
- Machines automatically detect nearby shapes, check for a valid recipe, destroy the input shapes, and produce the resulting shape based on the recipe.

### Level Setup
- A level featuring at least four machines, each equipped with unique recipes (minimum of ten recipes in the data table).

### Multiplayer Support
- The project runs on a dedicated server with at least one client.
- Utilizes push model replication for all network operations.

### Interactive Button System
- Button actors that respond to two different input keys (e.g., left and right mouse buttons).
- Clients can interact with the button, sending commands to the server.
- Each machine is equipped with an attached button.

### Gameplay Mechanics
- Pressing the button with the first input triggers a random recipe without needing ingredients, with the result being spawned server-side and replicated to the client.
- The second input toggles the machine on or off, with visual or auditory feedback indicating the machine's status.
- Successful recipe completions trigger effects near the output item, while failed attempts due to the machine being off provide player feedback.
