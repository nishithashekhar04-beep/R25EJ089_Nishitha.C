/**
 * Project Name: 2D Graphics Editor (Mini-Project in C)
 * File Name: main.c
 * 
 * Description:
 * A structured, modular, console-based 2D Graphics Editor written in standard C.
 * It uses a 20x40 character matrix as a canvas. Graphics objects (Rectangles, Lines, 
 * Triangles, Circles) are stored as structured models in an object array, permitting 
 * interactive drawing, deletion, modification, and canvas rendering.
 *
 * Objectives Met:
 * 1. 2D Canvas creation (20x40 char array initialized with '_').
 * 2. Complete menu-driven control (switch-case).
 * 3. Structured object representation (enums, structures, unions).
 * 4. Coordinate validation to remain within canvas boundary constraints.
 * 5. Dynamic deletion and modification (moving objects by deletion and redrawing).
 * 6. Clean modular design and code comments.
 */

#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 20
#define WIDTH 40
#define MAX_OBJECTS 100

/* Enumeration to define different shape types */
typedef enum {
    RECTANGLE = 1,
    LINE = 2,
    TRIANGLE = 3,
    CIRCLE = 4
} ShapeType;

/* Structure to store Rectangle attributes */
typedef struct {
    int x;       /* Top-left column coordinate (0 to WIDTH-1) */
    int y;       /* Top-left row coordinate (0 to HEIGHT-1) */
    int width;   /* Width of the rectangle */
    int height;  /* Height of the rectangle */
} Rectangle;

/* Structure to store Horizontal Line attributes */
typedef struct {
    int x;       /* Starting column coordinate (0 to WIDTH-1) */
    int y;       /* Row coordinate (0 to HEIGHT-1) */
    int length;  /* Length of the horizontal line */
} Line;

/* Structure to store Right-Angled Triangle attributes */
typedef struct {
    int x;       /* Top-left corner column coordinate (0 to WIDTH-1) */
    int y;       /* Top-left corner row coordinate (0 to HEIGHT-1) */
    int height;  /* Height and base length of the triangle */
} Triangle;

/* Structure to store Circle attributes */
typedef struct {
    int cx;      /* Center column coordinate (0 to WIDTH-1) */
    int cy;      /* Center row coordinate (0 to HEIGHT-1) */
    int r;       /* Radius of the circle */
} Circle;

/* Structure to store high-level Graphic Object information */
typedef struct {
    int id;               /* Unique identification number for the object */
    ShapeType type;       /* The type of shape (Rectangle, Line, etc.) */
    int isActive;         /* Status flag: 1 if active/visible, 0 if deleted */
    union {
        Rectangle rect;
        Line line;
        Triangle tri;
        Circle circ;
    } data;               /* Union containing shape-specific dimensions */
} GraphicObject;

/* Global arrays and tracking variables */
GraphicObject objects[MAX_OBJECTS];
int objectCount = 0;
char canvas[HEIGHT][WIDTH];

/* Function Prototypes */
void initializeCanvas(char canvas[HEIGHT][WIDTH]);
void displayCanvas(char canvas[HEIGHT][WIDTH]);
void drawRectangle(char canvas[HEIGHT][WIDTH], int x, int y, int width, int height);
void drawLine(char canvas[HEIGHT][WIDTH], int x, int y, int length);
void drawTriangle(char canvas[HEIGHT][WIDTH], int x, int y, int height);
void drawCircle(char canvas[HEIGHT][WIDTH], int cx, int cy, int r);
void deleteObject(char canvas[HEIGHT][WIDTH], int x, int y);
void modifyObject(char canvas[HEIGHT][WIDTH], int x, int y);

/* Helper Functions */
void clearInputBuffer(void);
void rebuildCanvas(char canvas[HEIGHT][WIDTH]);
int isPointInObject(GraphicObject obj, int px, int py);
int findObjectAt(int px, int py);
void compactObjects(void);

/* Shape Validation Helpers */
int validateRectangle(int x, int y, int width, int height);
int validateLine(int x, int y, int length);
int validateTriangle(int x, int y, int height);
int validateCircle(int cx, int cy, int r);

/**
 * Main execution loop managing user menus, reading parameters, and invoking procedures.
 */
int main(void) {
    int choice;
    initializeCanvas(canvas);
    
    printf("=========================================\n");
    printf("      WELCOME TO THE 2D GRAPHICS EDITOR   \n");
    printf("=========================================\n");
    printf("Canvas size: %d Columns x %d Rows\n", WIDTH, HEIGHT);
    printf("Coordinates start from (0,0) at top-left.\n");
    
    displayCanvas(canvas);

    while (1) {
        printf("--- Main Menu ---\n");
        printf("1. Draw Rectangle\n");
        printf("2. Draw Line\n");
        printf("3. Draw Triangle\n");
        printf("4. Draw Circle\n");
        printf("5. Delete Object\n");
        printf("6. Modify Object (Move)\n");
        printf("7. Display Canvas\n");
        printf("8. Reset Canvas\n");
        printf("9. Exit\n");
        printf("Select an option (1-9): ");

        if (scanf("%d", &choice) != 1) {
            printf("\nError: Invalid option selection. Please enter numbers only.\n\n");
            clearInputBuffer();
            continue;
        }

        switch (choice) {
            case 1: {
                int x, y, w, h;
                printf("Enter top-left X (Column 0-%d): ", WIDTH - 1);
                if (scanf("%d", &x) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter top-left Y (Row 0-%d): ", HEIGHT - 1);
                if (scanf("%d", &y) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter width: ");
                if (scanf("%d", &w) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter height: ");
                if (scanf("%d", &h) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }

                if (validateRectangle(x, y, w, h)) {
                    if (objectCount >= MAX_OBJECTS) {
                        compactObjects();
                    }
                    if (objectCount < MAX_OBJECTS) {
                        GraphicObject obj;
                        obj.id = objectCount + 1;
                        obj.type = RECTANGLE;
                        obj.isActive = 1;
                        obj.data.rect.x = x;
                        obj.data.rect.y = y;
                        obj.data.rect.width = w;
                        obj.data.rect.height = h;
                        
                        objects[objectCount++] = obj;
                        rebuildCanvas(canvas);
                        printf("\nRectangle added successfully!\n");
                        displayCanvas(canvas);
                    } else {
                        printf("\nError: Maximum object capacity reached. Delete some shapes first.\n\n");
                    }
                }
                break;
            }
            case 2: {
                int x, y, len;
                printf("Enter starting X (Column 0-%d): ", WIDTH - 1);
                if (scanf("%d", &x) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter Y (Row 0-%d): ", HEIGHT - 1);
                if (scanf("%d", &y) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter line length: ");
                if (scanf("%d", &len) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }

                if (validateLine(x, y, len)) {
                    if (objectCount >= MAX_OBJECTS) {
                        compactObjects();
                    }
                    if (objectCount < MAX_OBJECTS) {
                        GraphicObject obj;
                        obj.id = objectCount + 1;
                        obj.type = LINE;
                        obj.isActive = 1;
                        obj.data.line.x = x;
                        obj.data.line.y = y;
                        obj.data.line.length = len;
                        
                        objects[objectCount++] = obj;
                        rebuildCanvas(canvas);
                        printf("\nHorizontal Line added successfully!\n");
                        displayCanvas(canvas);
                    } else {
                        printf("\nError: Maximum object capacity reached.\n\n");
                    }
                }
                break;
            }
            case 3: {
                int x, y, h;
                printf("Enter top-left X (Column 0-%d): ", WIDTH - 1);
                if (scanf("%d", &x) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter top-left Y (Row 0-%d): ", HEIGHT - 1);
                if (scanf("%d", &y) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter height of the triangle: ");
                if (scanf("%d", &h) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }

                if (validateTriangle(x, y, h)) {
                    if (objectCount >= MAX_OBJECTS) {
                        compactObjects();
                    }
                    if (objectCount < MAX_OBJECTS) {
                        GraphicObject obj;
                        obj.id = objectCount + 1;
                        obj.type = TRIANGLE;
                        obj.isActive = 1;
                        obj.data.tri.x = x;
                        obj.data.tri.y = y;
                        obj.data.tri.height = h;
                        
                        objects[objectCount++] = obj;
                        rebuildCanvas(canvas);
                        printf("\nRight-angled Triangle added successfully!\n");
                        displayCanvas(canvas);
                    } else {
                        printf("\nError: Maximum object capacity reached.\n\n");
                    }
                }
                break;
            }
            case 4: {
                int cx, cy, r;
                printf("Enter center X (Column 0-%d): ", WIDTH - 1);
                if (scanf("%d", &cx) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter center Y (Row 0-%d): ", HEIGHT - 1);
                if (scanf("%d", &cy) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter radius: ");
                if (scanf("%d", &r) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }

                if (validateCircle(cx, cy, r)) {
                    if (objectCount >= MAX_OBJECTS) {
                        compactObjects();
                    }
                    if (objectCount < MAX_OBJECTS) {
                        GraphicObject obj;
                        obj.id = objectCount + 1;
                        obj.type = CIRCLE;
                        obj.isActive = 1;
                        obj.data.circ.cx = cx;
                        obj.data.circ.cy = cy;
                        obj.data.circ.r = r;
                        
                        objects[objectCount++] = obj;
                        rebuildCanvas(canvas);
                        printf("\nCircle added successfully!\n");
                        displayCanvas(canvas);
                    } else {
                        printf("\nError: Maximum object capacity reached.\n\n");
                    }
                }
                break;
            }
            case 5: {
                int px, py;
                printf("Enter X coordinate of object to delete (0-%d): ", WIDTH - 1);
                if (scanf("%d", &px) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter Y coordinate of object to delete (0-%d): ", HEIGHT - 1);
                if (scanf("%d", &py) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }

                deleteObject(canvas, px, py);
                displayCanvas(canvas);
                break;
            }
            case 6: {
                int px, py;
                printf("Enter X coordinate of object to modify/move (0-%d): ", WIDTH - 1);
                if (scanf("%d", &px) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }
                printf("Enter Y coordinate of object to modify/move (0-%d): ", HEIGHT - 1);
                if (scanf("%d", &py) != 1) { clearInputBuffer(); printf("Invalid input.\n"); break; }

                modifyObject(canvas, px, py);
                displayCanvas(canvas);
                break;
            }
            case 7:
                displayCanvas(canvas);
                break;
            case 8:
                objectCount = 0;
                initializeCanvas(canvas);
                printf("\nCanvas and all objects have been reset.\n");
                displayCanvas(canvas);
                break;
            case 9:
                printf("\nExiting 2D Graphics Editor. Goodbye!\n");
                exit(0);
            default:
                printf("\nError: Option out of range (1-9). Try again.\n\n");
        }
    }
    return 0;
}

/**
 * Initializes the canvas 2D matrix by placing the boundary/empty character '_' everywhere.
 */
void initializeCanvas(char canvas[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            canvas[i][j] = '_';
        }
    }
}

/**
 * Renders the canvas to the stdout terminal.
 * Includes column indices at the top and row indices on the left for easy alignment.
 */
void displayCanvas(char canvas[HEIGHT][WIDTH]) {
    printf("\n   ");
    for (int j = 0; j < WIDTH; j++) {
        printf("%d", j % 10);
    }
    printf("\n");
    for (int i = 0; i < HEIGHT; i++) {
        printf("%2d ", i);
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", canvas[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Draws the outline border of a rectangle on the canvas using '*' characters.
 */
void drawRectangle(char canvas[HEIGHT][WIDTH], int x, int y, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int px = x + j;
            int py = y + i;
            if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                /* Only set pixel if it is on the border of the rectangle */
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                    canvas[py][px] = '*';
                }
            }
        }
    }
}

/**
 * Draws a horizontal line on the canvas using '*' characters.
 */
void drawLine(char canvas[HEIGHT][WIDTH], int x, int y, int length) {
    for (int j = 0; j < length; j++) {
        int px = x + j;
        if (px >= 0 && px < WIDTH && y >= 0 && y < HEIGHT) {
            canvas[y][px] = '*';
        }
    }
}

/**
 * Draws a right-angled triangle filled with '*' characters.
 * The triangle's top-left corner of the bounding box is at (x, y).
 */
void drawTriangle(char canvas[HEIGHT][WIDTH], int x, int y, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j <= i; j++) {
            int px = x + j;
            int py = y + i;
            if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                canvas[py][px] = '*';
            }
        }
    }
}

/**
 * Draws a circle on the canvas using the equation: (x - cx)^2 + (y - cy)^2 <= r^2.
 */
void drawCircle(char canvas[HEIGHT][WIDTH], int cx, int cy, int r) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy <= r * r) {
                canvas[y][x] = '*';
            }
        }
    }
}

/**
 * Deletes an object that contains the coordinate (x, y) by changing its status to inactive.
 * It then rebuilds the canvas by re-drawing only active shapes.
 */
void deleteObject(char canvas[HEIGHT][WIDTH], int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        printf("Error: Input coordinates are outside the canvas range.\n");
        return;
    }
    
    int index = findObjectAt(x, y);
    if (index != -1) {
        objects[index].isActive = 0;
        rebuildCanvas(canvas);
        printf("\nSuccess: Deleted Object ID %d (Type %d) found at (%d, %d).\n", 
               objects[index].id, objects[index].type, x, y);
    } else {
        /* Fallback: If no structured object contains it, check if pixel is '*' and delete it directly */
        if (canvas[y][x] == '*') {
            canvas[y][x] = '_';
            printf("\nNotice: No structured object covers (%d, %d). Pixel erased directly.\n", x, y);
        } else {
            printf("\nNotice: No shape or drawn pixel exists at coordinates (%d, %d).\n", x, y);
        }
    }
}

/**
 * Modifies an existing object's coordinates.
 * Finds the object covering (x, y), asks user for new coordinates,
 * validates the new coordinates, deletes the old object, and spawns the redrawn one.
 */
void modifyObject(char canvas[HEIGHT][WIDTH], int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        printf("Error: Input coordinates are outside the canvas range.\n");
        return;
    }
    
    int index = findObjectAt(x, y);
    if (index == -1) {
        printf("\nError: No shape detected at (%d, %d) to modify.\n", x, y);
        return;
    }

    printf("\nTarget Shape Found: ");
    switch (objects[index].type) {
        case RECTANGLE:
            printf("Rectangle [ID: %d] at col=%d, row=%d, size=%dx%d\n", 
                   objects[index].id, objects[index].data.rect.x, objects[index].data.rect.y,
                   objects[index].data.rect.width, objects[index].data.rect.height);
            break;
        case LINE:
            printf("Line [ID: %d] at col=%d, row=%d, length=%d\n", 
                   objects[index].id, objects[index].data.line.x, objects[index].data.line.y,
                   objects[index].data.line.length);
            break;
        case TRIANGLE:
            printf("Triangle [ID: %d] at col=%d, row=%d, height=%d\n", 
                   objects[index].id, objects[index].data.tri.x, objects[index].data.tri.y,
                   objects[index].data.tri.height);
            break;
        case CIRCLE:
            printf("Circle [ID: %d] centered at col=%d, row=%d, radius=%d\n", 
                   objects[index].id, objects[index].data.circ.cx, objects[index].data.circ.cy,
                   objects[index].data.circ.r);
            break;
    }

    int newX, newY;
    printf("Enter new X coordinate (Column 0-%d): ", WIDTH - 1);
    if (scanf("%d", &newX) != 1) {
        clearInputBuffer();
        printf("Invalid input. Modification cancelled.\n");
        return;
    }
    printf("Enter new Y coordinate (Row 0-%d): ", HEIGHT - 1);
    if (scanf("%d", &newY) != 1) {
        clearInputBuffer();
        printf("Invalid input. Modification cancelled.\n");
        return;
    }

    /* Validate new positioning to ensure shape doesn't drift out of bounds */
    int isValid = 0;
    switch (objects[index].type) {
        case RECTANGLE:
            isValid = validateRectangle(newX, newY, objects[index].data.rect.width, objects[index].data.rect.height);
            break;
        case LINE:
            isValid = validateLine(newX, newY, objects[index].data.line.length);
            break;
        case TRIANGLE:
            isValid = validateTriangle(newX, newY, objects[index].data.tri.height);
            break;
        case CIRCLE:
            isValid = validateCircle(newX, newY, objects[index].data.circ.r);
            break;
    }

    if (!isValid) {
        printf("\nModification failed: The new coordinates put the shape outside the canvas boundary.\n");
        return;
    }

    /* Perform deletion & redraw: deactivate old object and create a new one */
    objects[index].isActive = 0;

    if (objectCount >= MAX_OBJECTS) {
        compactObjects();
    }

    if (objectCount < MAX_OBJECTS) {
        GraphicObject newObj;
        newObj.id = objectCount + 1;
        newObj.type = objects[index].type;
        newObj.isActive = 1;

        switch (objects[index].type) {
            case RECTANGLE:
                newObj.data.rect.x = newX;
                newObj.data.rect.y = newY;
                newObj.data.rect.width = objects[index].data.rect.width;
                newObj.data.rect.height = objects[index].data.rect.height;
                break;
            case LINE:
                newObj.data.line.x = newX;
                newObj.data.line.y = newY;
                newObj.data.line.length = objects[index].data.line.length;
                break;
            case TRIANGLE:
                newObj.data.tri.x = newX;
                newObj.data.tri.y = newY;
                newObj.data.tri.height = objects[index].data.tri.height;
                break;
            case CIRCLE:
                newObj.data.circ.cx = newX;
                newObj.data.circ.cy = newY;
                newObj.data.circ.r = objects[index].data.circ.r;
                break;
        }

        objects[objectCount++] = newObj;
        rebuildCanvas(canvas);
        printf("\nSuccess: Object modified and redrawn at new location (%d, %d).\n", newX, newY);
    } else {
        /* If array is completely full even after compaction, revert original's active status */
        objects[index].isActive = 1;
        printf("\nError: Cannot complete modification. Object memory is full.\n");
    }
}

/**
 * Helper to empty standard input buffer, preventing infinite loops on invalid input.
 */
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != EOF && c != '\n');
}

/**
 * Iterates through all active objects to reconstruct canvas grid state from scratch.
 */
void rebuildCanvas(char canvas[HEIGHT][WIDTH]) {
    initializeCanvas(canvas);
    for (int i = 0; i < objectCount; i++) {
        if (objects[i].isActive) {
            switch (objects[i].type) {
                case RECTANGLE:
                    drawRectangle(canvas, objects[i].data.rect.x, objects[i].data.rect.y,
                                  objects[i].data.rect.width, objects[i].data.rect.height);
                    break;
                case LINE:
                    drawLine(canvas, objects[i].data.line.x, objects[i].data.line.y,
                             objects[i].data.line.length);
                    break;
                case TRIANGLE:
                    drawTriangle(canvas, objects[i].data.tri.x, objects[i].data.tri.y,
                                 objects[i].data.tri.height);
                    break;
                case CIRCLE:
                    drawCircle(canvas, objects[i].data.circ.cx, objects[i].data.circ.cy,
                               objects[i].data.circ.r);
                    break;
            }
        }
    }
}

/**
 * Checks geometrically if point (px, py) is inside/on the boundaries of a given active GraphicObject.
 */
int isPointInObject(GraphicObject obj, int px, int py) {
    if (!obj.isActive) {
        return 0;
    }
    
    switch (obj.type) {
        case RECTANGLE: {
            Rectangle r = obj.data.rect;
            /* Rectangle bounding-box check */
            return (px >= r.x && px < r.x + r.width && py >= r.y && py < r.y + r.height);
        }
        case LINE: {
            Line l = obj.data.line;
            /* Horizontal line check */
            return (py == l.y && px >= l.x && px < l.x + l.length);
        }
        case TRIANGLE: {
            Triangle t = obj.data.tri;
            /* Bottom-left aligned right-angled triangle cell check */
            return (py >= t.y && py < t.y + t.height && px >= t.x && px <= t.x + (py - t.y));
        }
        case CIRCLE: {
            Circle c = obj.data.circ;
            /* Circle boundary/area check using (x-cx)^2 + (y-cy)^2 <= r^2 */
            int dx = px - c.cx;
            int dy = py - c.cy;
            return (dx * dx + dy * dy <= c.r * c.r);
        }
    }
    return 0;
}

/**
 * Finds the first active object that overlaps with coordinate (px, py) in z-order (newest first).
 * Returns index in objects array, or -1 if not found.
 */
int findObjectAt(int px, int py) {
    for (int i = objectCount - 1; i >= 0; i--) {
        if (objects[i].isActive && isPointInObject(objects[i], px, py)) {
            return i;
        }
    }
    return -1;
}

/**
 * Removes inactive (deleted) elements from the objects array to prevent storage exhaustion.
 */
void compactObjects(void) {
    int writeIdx = 0;
    for (int i = 0; i < objectCount; i++) {
        if (objects[i].isActive) {
            if (writeIdx != i) {
                objects[writeIdx] = objects[i];
            }
            writeIdx++;
        }
    }
    objectCount = writeIdx;
}

/**
 * Bounds-validation helper for Rectangles.
 */
int validateRectangle(int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) {
        printf("\nError: Dimensions must be greater than zero.\n");
        return 0;
    }
    if (x < 0 || y < 0 || x + width > WIDTH || y + height > HEIGHT) {
        printf("\nError: Rectangle goes out of canvas bounds.\n");
        return 0;
    }
    return 1;
}

/**
 * Bounds-validation helper for Horizontal Lines.
 */
int validateLine(int x, int y, int length) {
    if (length <= 0) {
        printf("\nError: Line length must be greater than zero.\n");
        return 0;
    }
    if (y < 0 || y >= HEIGHT || x < 0 || x + length > WIDTH) {
        printf("\nError: Line goes out of canvas bounds.\n");
        return 0;
    }
    return 1;
}

/**
 * Bounds-validation helper for Triangles.
 */
int validateTriangle(int x, int y, int height) {
    if (height <= 0) {
        printf("\nError: Triangle height must be greater than zero.\n");
        return 0;
    }
    if (x < 0 || y < 0 || x + height > WIDTH || y + height > HEIGHT) {
        printf("\nError: Triangle goes out of canvas bounds.\n");
        return 0;
    }
    return 1;
}

/**
 * Bounds-validation helper for Circles.
 */
int validateCircle(int cx, int cy, int r) {
    if (r <= 0) {
        printf("\nError: Circle radius must be greater than zero.\n");
        return 0;
    }
    if (cx - r < 0 || cx + r >= WIDTH || cy - r < 0 || cy + r >= HEIGHT) {
        printf("\nError: Circle boundaries exceed canvas bounds.\n");
        return 0;
    }
    return 1;
}
