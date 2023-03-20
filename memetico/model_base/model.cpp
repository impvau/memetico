
#include <memetico/model_base/model.h>

PrintType Model::FORMAT = PrintExcel;

string Model::OBJECTIVE_NAME = "";

double (*Model::OBJECTIVE)(Model*, DataSet*, vector<size_t>&) = nullptr;

