#ifndef __Result_File_hdf5_h__
#define __Result_File_hdf5_h__

#include "hdf5.h"

#include "ResultFile.h"

class ResultFile_hdf5 : public ResultFile
{
protected:
	hid_t file_id;
	hid_t md_grp_id; // model data id
	hid_t th_grp_id; // time history id

public:
	ResultFile_hdf5();
	~ResultFile_hdf5();

	// open hdf5 file
	int create(const char *file_name, bool over_write = true);
	int open(const char *file_name, bool read_only = true);
	void close();
	inline hid_t get_file_id() noexcept { return file_id; }
	inline bool is_open() { return file_id >= 0 ? true : false; }

	// group
	hid_t create_group(hid_t parent_id, const char *name);
	hid_t open_group(hid_t parent_id, const char *name);
	void close_group(hid_t id);
	bool has_group(hid_t parent_id, const char *name);

	// dataset
	hid_t open_dataset(hid_t parent_id, const char *name);
	void close_dataset(hid_t id);
	bool has_dataset(hid_t parent_id, const char *name);

	int write_dataset(hid_t grp_id, const char *dset_name, 
					  size_t num, double *data);
	int write_dataset(hid_t grp_id, const char* dset_name,
		size_t num, void* data, hid_t datatype_id);
	int write_dataset(hid_t grp_id, const char *dset_name,
		size_t row_num, size_t col_num, double *data);
	int write_dataset(hid_t grp_id, const char *dset_name,
		size_t row_num,	size_t col_num, unsigned long long *data);

	int read_dataset(hid_t grp_id, const char *dset_name,
					 size_t num, double *data);
	int read_dataset(hid_t grp_id, const char* dset_name,
		size_t num, void* data, hid_t datatype_id);
	int read_dataset(hid_t grp_id, const char *dset_name,
		size_t row_num, size_t col_num, double *data);
	int read_dataset(hid_t grp_id, const char *dset_name,
		size_t row_num, size_t col_num, unsigned long long *data);

	// attributes
	// whether exists
	bool has_attribute(hid_t grp_id, const char* name);
	// write
	int write_attribute(hid_t grp_id, const char *name, double value);
	int write_attribute(hid_t grp_id, const char* name, float value);
	int write_attribute(hid_t grp_id, const char *name, size_t value);
	int write_attribute(hid_t grp_id, const char* name, uint32_t value);
	int write_attribute(hid_t grp_id, const char *name, size_t num, const char *str);
	// read
	int read_attribute(hid_t grp_id, const char *name, double &value);
	int read_attribute(hid_t grp_id, const char* name, float &value);
	int read_attribute(hid_t grp_id, const char *name, size_t &value);
	int read_attribute(hid_t grp_id, const char* name, uint32_t &value);
	int read_attribute(hid_t grp_id, const char *name, size_t num, const char *str);

	// model data group
	hid_t get_model_data_grp_id();
	// time history group
	hid_t get_time_history_grp_id();
};

#endif