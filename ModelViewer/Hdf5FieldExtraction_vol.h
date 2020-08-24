#ifndef __Hdf5_Field_Extraction_vol_h__
#define __Hdf5_Field_Extraction_vol_h__

#include "Hdf5FieldExtraction.h"

class Hdf5FieldExtraction_vol : public Hdf5FieldExtraction
{
protected:
	bool all_fld_is_found;
	size_t m_offset;
	size_t density_offset;

public:
	Hdf5FieldExtraction_vol() : all_fld_is_found(false) {}
	Hdf5FieldExtraction_vol(Hdf5DataLoader& loader) :
		Hdf5FieldExtraction(loader), all_fld_is_found(false) {}
	~Hdf5FieldExtraction_vol() {}

	bool validate_data_type() override;
	int extract_pcl_fld_data(double* pcl_fld_data) override;
};

#endif