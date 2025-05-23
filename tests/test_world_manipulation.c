#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "engine/world.h"
#include "engine/player.h" // Included for PlayerState if used by world or for context
#include "engine/constants.h"
#include "engine/cube.h"   // For initCubeVBOs if needed by world gen or drawing logic called by tests

// Forward declarations for test functions
void test_place_block_empty_space();
void test_destroy_single_block();
void test_place_block_on_existing_block_fails();
void test_place_block_updates_obstructed_status();
void test_destroy_block_updates_neighbors_obstruction();

void reset_world_for_test() {
    WorldState* ws = getWorldStateGlobal();
    // Check if chunks were actually allocated before trying to free them
    if (ws->chunks != NULL) {
        removeWorld(); // Clears the global worldState, including freeing chunks
    }
    // generateWorld should re-allocate and set up the global worldState.
    // Ensure worldState.chunks is NULL or points to freed memory before generateWorld.
    // removeWorld should set ws->chunks to NULL or ws->chunkCount to 0.
    // For this setup, we assume generateWorld correctly reinitializes the global worldState.
    generateWorld(); 
}

int main() {
    printf("Starting world manipulation tests...\n");
    
    // initCubeVBOs(); // Only if strictly needed by functions called within tests (e.g. generateWorld indirectly)
                     // For pure logic tests of place/destroy, this might not be necessary.
                     // Let's assume for now it's not needed.

    reset_world_for_test();
    test_place_block_empty_space();

    reset_world_for_test();
    test_destroy_single_block();

    reset_world_for_test();
    test_place_block_on_existing_block_fails();
    
    reset_world_for_test();
    test_place_block_updates_obstructed_status();
    
    reset_world_for_test();
    test_destroy_block_updates_neighbors_obstruction();

    // Final cleanup of the last test's world state
    WorldState* ws = getWorldStateGlobal();
    if (ws->chunks != NULL) {
        removeWorld();
    }
    // freeCubeVBOs(); // Counterpart to initCubeVBOs

    printf("All world manipulation tests completed successfully!\n");
    return 0;
}

// Test case implementations will be added below

void test_place_block_empty_space() {
    printf("Running test_place_block_empty_space...\n");
    WorldState* ws = getWorldStateGlobal();
    int x = 10, y = 10, z = 10; // Choose coordinates away from typical world generation
    int type = 2;

    placeBlock(ws, x, y, z, type);
    GameElement* block = getBlockAtGlobal(ws, x, y, z);

    assert(block != NULL);
    if (block != NULL) { // Proceed only if block is not NULL
        assert(block->elementType == type);
        // A newly placed block in an empty area (far from others) should not be obstructed.
        // placeBlock itself is responsible for calculating this.
        assert(block->isObstructed == false);
    }
    printf("test_place_block_empty_space PASSED\n");
}

void test_destroy_single_block() {
    printf("Running test_destroy_single_block...\n");
    WorldState* ws = getWorldStateGlobal();
    int x = 5, y = 5, z = 5;
    int type = 1;

    // Place a block
    placeBlock(ws, x, y, z, type);
    GameElement* block = getBlockAtGlobal(ws, x, y, z);
    assert(block != NULL && block->elementType == type);

    // Destroy the block
    destroyBlock(ws, x, y, z);
    block = getBlockAtGlobal(ws, x, y, z); // Re-fetch the block
    // getBlockAtGlobal returns NULL for air blocks (elementType == 0)
    assert(block == NULL); 

    printf("test_destroy_single_block PASSED\n");
}

void test_place_block_on_existing_block_fails() {
    printf("Running test_place_block_on_existing_block_fails...\n");
    WorldState* ws = getWorldStateGlobal();
    int x = 6, y = 6, z = 6;
    int type1 = 1;
    int type2 = 2;

    // Place initial block
    placeBlock(ws, x, y, z, type1);
    GameElement* block = getBlockAtGlobal(ws, x, y, z);
    assert(block != NULL && block->elementType == type1);

    // Attempt to place another block at the same location
    placeBlock(ws, x, y, z, type2);
    block = getBlockAtGlobal(ws, x, y, z); // Re-fetch
    
    // Assert that the original block is still there and its type has not changed
    assert(block != NULL && block->elementType == type1);

    printf("test_place_block_on_existing_block_fails PASSED\n");
}

void test_place_block_updates_obstructed_status() {
    printf("Running test_place_block_updates_obstructed_status...\n");
    WorldState* ws = getWorldStateGlobal();
    // Define coordinates for a central block and its neighbors
    int cx = 3, cy = 3, cz = 3;

    // Place the central block A
    placeBlock(ws, cx, cy, cz, 1);
    GameElement* blockA = getBlockAtGlobal(ws, cx, cy, cz);
    assert(blockA != NULL);
    assert(blockA->isObstructed == false); // Initially not obstructed

    // Place 5 neighbors (not enough to obstruct)
    placeBlock(ws, cx + 1, cy, cz, 1); // Right
    placeBlock(ws, cx - 1, cy, cz, 1); // Left
    placeBlock(ws, cx, cy + 1, cz, 1); // Top
    placeBlock(ws, cx, cy - 1, cz, 1); // Bottom
    placeBlock(ws, cx, cy, cz + 1, 1); // Front
    
    blockA = getBlockAtGlobal(ws, cx, cy, cz); // Re-fetch A
    assert(blockA->isObstructed == false); // Should still not be obstructed

    // Place the 6th neighbor (back)
    placeBlock(ws, cx, cy, cz - 1, 1); // Back
    
    blockA = getBlockAtGlobal(ws, cx, cy, cz); // Re-fetch A
    assert(blockA->isObstructed == true); // Now A should be obstructed

    printf("test_place_block_updates_obstructed_status PASSED\n");
}

void test_destroy_block_updates_neighbors_obstruction() {
    printf("Running test_destroy_block_updates_neighbors_obstruction...\n");
    WorldState* ws = getWorldStateGlobal();
    int ax = 1, ay = 1, az = 1; // Block A
    int bx = 2, by = 1, bz = 1; // Block B (neighbor of A)

    // Place A and B
    placeBlock(ws, ax, ay, az, 1); // Block A
    placeBlock(ws, bx, by, bz, 1); // Block B

    GameElement* blockB = getBlockAtGlobal(ws, bx, by, bz);
    assert(blockB != NULL);
    assert(blockB->isObstructed == false); // B is not initially obstructed by only A

    // Surround B to make it obstructed. A is one of these neighbors.
    // Neighbors of B (2,1,1):
    // (1,1,1) -> A (already placed)
    // (3,1,1) -> Right of B
    // (2,2,1) -> Top of B
    // (2,0,1) -> Bottom of B
    // (2,1,2) -> Front of B
    // (2,1,0) -> Back of B
    placeBlock(ws, bx + 1, by, bz, 1); // Right
    placeBlock(ws, bx, by + 1, bz, 1); // Top
    placeBlock(ws, bx, by - 1, bz, 1); // Bottom
    placeBlock(ws, bx, by, bz + 1, 1); // Front
    placeBlock(ws, bx, by, bz - 1, 1); // Back

    blockB = getBlockAtGlobal(ws, bx, by, bz); // Re-fetch B
    assert(blockB->isObstructed == true); // B should now be obstructed

    // Destroy block A
    destroyBlock(ws, ax, ay, az);

    blockB = getBlockAtGlobal(ws, bx, by, bz); // Re-fetch B
    assert(blockB != NULL); // B should still exist
    assert(blockB->isObstructed == false); // B should no longer be obstructed

    printf("test_destroy_block_updates_neighbors_obstruction PASSED\n");
}
