#include "CSpreadsheet.hpp"
#include <csignal>
#include "PostfixExpressionBuilder.hpp"

//----------------METHODS OF CLASS CSPREADHSEET-----------------

bool CSpreadsheet::setCell(CPos pos, std::string contents) {
    //check for empty string
    if (contents.empty()) {
        //empty string -> setcell failed
        return false;
    }

    //allocate CellValue
    CellValue result;

    //parse expression by PostfixExpressionBuilder -> create m_parsed_vector saved in CellValue
    try {
        PostfixExpressionBuilder builder;
        parseExpression(contents, builder);
        result = builder.getCell();
    } catch (...) {
        //invalid expression -> setcell failed
        return false;
    }


    //clear cache because of the reference dependencies
    m_cache_map.clear();

    //canonical value save
    result.m_canonical_value = contents;

    //save or overwrite new cell to spreadsheet
    m_sheet[pos.pos()] = result;
    return true;
}


CValue CSpreadsheet::getValue(CPos pos) {
    //this function can be called recursively so it uses m_cache_map located in CSpreadsheet to determine if it's in
    //infinite loop and also to optimize reading values of already calculated cells

    //check for not existing cell
    if (!m_sheet.contains(pos.pos())) {
        //non-existent cell -> invalid value
        return CValue();
    }

    //check if result is already stored in cache
    if (m_cache_map.contains(pos.pos())) {
        //found in cache -> instant return
        return m_cache_map[pos.pos()];
    }


    //marked as invalid value -> used for DFS
    m_cache_map[pos.pos()] = CValue();


    //calculate value with operators and values in m_parsed_vector -> result saved to top of m_calculated_stack
    for (auto &elem: m_sheet[pos.pos()].m_parsed_vector) {
        elem->operation(*this, pos);
    }


    //read result
    try {
        //save result to cache
        CValue x = m_sheet[pos.pos()].m_calculated_stack.top().second;
        m_cache_map[pos.pos()] = x;
    } catch (...) {
        //invalid value
        return CValue();
    }

    //delete result from m_calculated stack
    m_sheet[pos.pos()].m_calculated_stack.pop();

    //return value from cache
    return m_cache_map[pos.pos()];
}

void CSpreadsheet::copyRect(CPos dst, CPos src, int w, int h) {
    //clear cache because of the reference dependencies
    m_cache_map.clear();

    CPos source = src;
    CPos destination = dst;

    std::vector<std::pair<CellValue, std::pair<unsigned long long int, unsigned long long int> > > copy_vec;

    //saves all CellValue from source rectangle with its destination address to copy_vec
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            std::pair shifted_source = {source.pos().first + x, source.pos().second + y};
            std::pair shifted_destination = {destination.pos().first + x, destination.pos().second + y};

            //saves non-empty cells from source and its destination
            if (m_sheet.contains(shifted_source) && shifted_source != shifted_destination) {
                CellValue to_be_pushed;
                to_be_pushed = m_sheet[shifted_source];
                copy_vec.push_back({to_be_pushed, shifted_destination});

                //adding shift for relative references
                (copy_vec.end() - 1)->first.m_shift.first += shifted_destination.first - shifted_source.first;
                (copy_vec.end() - 1)->first.m_shift.second += shifted_destination.second - shifted_source.second;
            }
            //empty cells in source are dealt directly by erasing their destination content
            if (!m_sheet.contains(shifted_source) && shifted_source != shifted_destination) {
                m_sheet.erase(shifted_destination);
            }
        }
    }

    //copy whole source rectangle to destination rectangle
    for (auto &elem: copy_vec) {
        m_sheet[elem.second] = elem.first;
    }
}

//function for checking fail and eof flags of stream, true = no flags, false = eof or fail flag
bool check_stream(const std::istream &is) {
    if (is.fail()) {
        return false;
    }
    if (is.eof()) {
        return false;
    }
    return true;
}

//function used for saving whole CSpreadsheet instance to std::ostream, true = successfuly saved, false = failed stream
bool CSpreadsheet::save(std::ostream &os) const {
    //check of stream fail flag
    if (os.fail()) {
        return false;
    }

    uint64_t len_of_mapa = m_sheet.size();
    std::stringstream tmp_stream;

    //writing content (address coordinates, shift vector, canonical value) of all cells to tmp_stream in binary
    tmp_stream.write(reinterpret_cast<char *>(&len_of_mapa), sizeof(len_of_mapa));
    for (const auto &elem: m_sheet) {
        uint64_t len_of_value = elem.second.m_canonical_value.size();
        tmp_stream.write(reinterpret_cast<const char *>(&elem.first.first), sizeof(elem.first.first));
        tmp_stream.write(reinterpret_cast<const char *>(&elem.first.second), sizeof(elem.first.second));
        tmp_stream.write(reinterpret_cast<const char *>(&elem.second.m_shift.first), sizeof(elem.second.m_shift.first));
        tmp_stream.write(reinterpret_cast<const char *>(&elem.second.m_shift.second),
                         sizeof(elem.second.m_shift.second));
        tmp_stream.write(reinterpret_cast<const char *>(&len_of_value), sizeof(len_of_value));
        tmp_stream.write(elem.second.m_canonical_value.c_str(), elem.second.m_canonical_value.size());
    }

    //calculating checksum of saved CSpreadsheet
    size_t hash = std::hash<std::string>{}(tmp_stream.str());

    //saving checksum with content to ostream
    os.write(reinterpret_cast<const char *>(&hash), sizeof(hash));
    os << tmp_stream.str();

    return true;
}

bool CSpreadsheet::load(std::istream &is) {
    //check for stream flags
    if (!check_stream(is)) {
        return false;
    }

    uint64_t len_of_mapa;
    uint64_t len_of_value;
    std::pair<unsigned long long int, unsigned long long int> key;
    std::pair<unsigned long long int, unsigned long long int> shift;
    std::string value;
    size_t hash;

    //read previously saved checksum
    is.read(reinterpret_cast<char *>(&hash), sizeof(hash));
    //check for stream flags
    if (!check_stream(is)) { return false; }

    //save rest of the content to tmp_stream
    std::stringstream tmp_stream;
    tmp_stream << is.rdbuf();
    //check for stream flags
    if (!check_stream(is)) { return false; }

    //calculate actual checksum
    size_t actual_hash = std::hash<std::string>{}(tmp_stream.str());

    //if checksums don't match -> corrupted content
    if (actual_hash != hash) {
        return false;
    }

    //load rest of the content to current instance of CSpreadsheet
    tmp_stream.read(reinterpret_cast<char *>(&len_of_mapa), sizeof(len_of_mapa));
    for (uint64_t i = 0; i < len_of_mapa; i++) {
        tmp_stream.read(reinterpret_cast<char *>(&key.first), sizeof(key.first));
        tmp_stream.read(reinterpret_cast<char *>(&key.second), sizeof(key.second));
        tmp_stream.read(reinterpret_cast<char *>(&shift.first), sizeof(shift.first));
        tmp_stream.read(reinterpret_cast<char *>(&shift.second), sizeof(shift.second));
        tmp_stream.read(reinterpret_cast<char *>(&len_of_value), sizeof(len_of_value));
        value.resize(len_of_value);
        tmp_stream.read(value.data(), len_of_value);
        std::cout << value << std::endl;

        //check for some setcell fails -> shouldn't happen but even checksum can be corrupted by someone...
        //set cell value
        if (!this->setCell(CPos(key), value)) {
            return false;
        }
        //save cell shift_vector
        this->m_sheet[key].m_shift = shift;
    }

    return true;
}

