template <typename T> class Matrix {
    private:
        std::vector<std::vector<T>> m;
        int rows;
        int cols;

    public:
        Matrix(int rows, int cols) : rows(rows), cols(cols) {}
        Matrix(std::vector<std::vector<T>> values) {
            rows = values.size();
            cols = values[0].size();

            /* Verify matrix is valid */
            for (int i=0; i<rows; ++i) {
                if (values[i].size() != cols) {
                    throw std::invalid_argument("Matrix rows are not all the same length.");
                }
            }
            m = values;
        }

        /* For accessing elements */
        std::vector<T> operator[] (int i) { return m[i]; }

        /* For read-only access */
        const std::vector<T> operator[] (int i) const { return m[i]; }

        void print() {
            for (int i=0; i<rows; ++i) {
                for (int j=0; j<cols; ++j) {
                    std::cout << m[i][j] << " ";
                }
                std::cout << "\n";
            }
        }

        int getRows() const { return rows; }
        int getCols() const { return cols; }
};