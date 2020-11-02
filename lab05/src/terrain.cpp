#include "terrain.h"

#include <math.h>
#include "gl/shaders/ShaderAttribLocations.h"

Terrain::Terrain() : m_numRows(100), m_numCols(m_numRows)
{
}


/**
 * Returns a pseudo-random value between -1.0 and 1.0 for the given row and column.
 */
float Terrain::randValue(int row, int col) {
    return -1.0 + 2.0 * glm::fract(sin(row * 127.1f + col * 311.7f) * 43758.5453123f);
}


/**
 * Returns the object-space position for the terrain vertex at the given row and column.
 */
glm::vec3 Terrain::getPosition(int row, int col) {
    glm::vec3 position;
    position.x = 10 * row/m_numRows - 5;
    position.y = 0;
    position.z = 10 * col/m_numCols - 5;

    // TODO: Adjust position.y using value noise.
    float row_f = row/20.f;
    float col_f = col/20.f;
    int new_row = glm::floor(row_f);
    int new_col = glm::floor(col_f);
    int next_row = new_row+1;
    int next_col = new_col+1;
    if (next_row >= 5)
        next_row = new_row;
    if (next_col >= 5)
        next_col = new_col;
    float val_A = randValue(new_row, new_col);
    float val_B = randValue(new_row, next_col);
    float val_C = randValue(next_row, new_col);
    float val_D = randValue(next_row, next_col);
    float t_col = glm::fract(col_f);
    float t_row = glm::fract(row_f);
    t_col = 3*pow(t_col, 2) - 2*pow(t_col, 3);
    t_row = 3*pow(t_row, 2) - 2*pow(t_row, 3);
    position.y += glm::mix(glm::mix(val_A, val_B, t_col),
                          glm::mix(val_C, val_D, t_col),
                          t_row);

    row_f = row/10.f;
    col_f = col/10.f;
    new_row = glm::floor(row_f);
    new_col = glm::floor(col_f);
    next_row = new_row+1;
    next_col = new_col+1;
    if (next_row >= 10)
        next_row = new_row;
    if (next_col >= 10)
        next_col = new_col;
    val_A = randValue(new_row, new_col);
    val_B = randValue(new_row, next_col);
    val_C = randValue(next_row, new_col);
    val_D = randValue(next_row, next_col);
    t_col = glm::fract(col_f);
    t_row = glm::fract(row_f);
    t_col = 3*pow(t_col, 2) - 2*pow(t_col, 3);
    t_row = 3*pow(t_row, 2) - 2*pow(t_row, 3);
    position.y += 0.5*glm::mix(glm::mix(val_A, val_B, t_col),
                          glm::mix(val_C, val_D, t_col),
                          t_row);

    row_f = row/2.5f;
    col_f = col/2.5f;
    new_row = glm::floor(row_f);
    new_col = glm::floor(col_f);
    next_row = new_row+1;
    next_col = new_col+1;
    if (next_row >= 40)
        next_row = new_row;
    if (next_col >= 40)
        next_col = new_col;
    val_A = randValue(new_row, new_col);
    val_B = randValue(new_row, next_col);
    val_C = randValue(next_row, new_col);
    val_D = randValue(next_row, next_col);
    t_col = glm::fract(col_f);
    t_row = glm::fract(row_f);
    t_col = 3*pow(t_col, 2) - 2*pow(t_col, 3);
    t_row = 3*pow(t_row, 2) - 2*pow(t_row, 3);
    position.y += 0.25*glm::mix(glm::mix(val_A, val_B, t_col),
                          glm::mix(val_C, val_D, t_col),
                          t_row);

    return position;
}


/**
 * Returns the normal vector for the terrain vertex at the given row and column.
 */
glm::vec3 Terrain::getNormal(int row, int col) {
    // TODO: Compute the normal at the given row and column using the positions of the
    //       neighboring vertices.
    if (row == 0 || row == 99 || col == 0 || col == 99)
        return glm::vec3(0, 1, 0);
    glm::vec3 normal = glm::vec3(0.0);
    glm::vec3 tmp_normal;
    glm::vec3 current = getPosition(row, col);
    normal += glm::normalize(glm::cross(getPosition(row-1, col-1)-current,
                                        getPosition(row-1, col)-current));
    normal += glm::normalize(glm::cross(getPosition(row-1, col)-current,
                                        getPosition(row-1, col+1)-current));
    normal += glm::normalize(glm::cross(getPosition(row-1, col+1)-current,
                                        getPosition(row, col+1)-current));
    normal += glm::normalize(glm::cross(getPosition(row, col+1)-current,
                                        getPosition(row+1, col+1)-current));
    normal += glm::normalize(glm::cross(getPosition(row+1, col+1)-current,
                                        getPosition(row+1, col)-current));
    normal += glm::normalize(glm::cross(getPosition(row+1, col)-current,
                                        getPosition(row+1, col-1)-current));
    normal += glm::normalize(glm::cross(getPosition(row+1, col-1)-current,
                                        getPosition(row, col-1)-current));
    normal += glm::normalize(glm::cross(getPosition(row, col-1)-current,
                                        getPosition(row-1, col-1)-current));
    return glm::normalize(normal);
}


/**
 * Initializes the terrain by storing positions and normals in a vertex buffer.
 */
void Terrain::init() {
    // TODO: Change from GL_LINE to GL_FILL in order to render full triangles instead of wireframe.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // Initializes a grid of vertices using triangle strips.
    int numVertices = (m_numRows - 1) * (2 * m_numCols + 2);
    std::vector<glm::vec3> data(2 * numVertices);
    int index = 0;
    for (int row = 0; row < m_numRows - 1; row++) {
        for (int col = m_numCols - 1; col >= 0; col--) {
            data[index++] = getPosition(row, col);
            data[index++] = getNormal  (row, col);
            data[index++] = getPosition(row + 1, col);
            data[index++] = getNormal  (row + 1, col);
        }
        data[index++] = getPosition(row + 1, 0);
        data[index++] = getNormal  (row + 1, 0);
        data[index++] = getPosition(row + 1, m_numCols - 1);
        data[index++] = getNormal  (row + 1, m_numCols - 1);
    }

    // Initialize OpenGLShape.
    m_shape = std::make_unique<OpenGLShape>();
    m_shape->setVertexData(&data[0][0], data.size() * 3, VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, numVertices);
    m_shape->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->setAttribute(ShaderAttrib::NORMAL, 3, sizeof(glm::vec3), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->buildVAO();
}


/**
 * Draws the terrain.
 */
void Terrain::draw()
{
    m_shape->draw();
}
