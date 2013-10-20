#pragma once
#ifndef HEADER_SIGNED_DISTANCE_FONT_XXX
#define HEADER_SIGNED_DISTANCE_FONT_XXX
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>

#pragma warning (push)
#pragma warning (disable : 4244)
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/text_format.h>

#pragma warning (pop)
#ifdef _WIN32
	#include <io.h>
#endif
#include "akjFont.pb.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <locale>


namespace akj {

struct kernPair {
	kernPair() {}
	~kernPair() {};
	float x;
	float y;
	short a;
	short b;
	bool operator < (const kernPair& other) const {
		return (a < other.a || (a == other.a && b < other.b)); };
	void saveToProtoBuf (sdff::akjFont::kernData* kernToSave) {
		kernToSave->set_x (x);
		kernToSave->set_y (y);
		kernToSave->set_a (static_cast<int> (a));
		kernToSave->set_b (static_cast<int> (b)); }; };
class SignedDistanceFieldFont {
	public:
	struct charData {
		int ID;
		int indx;
		float xOffset ;//* scale_factor  | Draw the glyph at X,Y offset
		float yOffset; //* scale_factor  | relative to the cursor, then
		float xAdvance;// * scale_factor | advance the cursor by this.
		void saveToProtoBuf (sdff::akjFont::charData* charToSave) {
			charToSave->set_id (ID);
			charToSave->set_indx (indx);
			charToSave->set_xoffset (xOffset);
			charToSave->set_yoffset (yOffset);
			charToSave->set_xadvance (xAdvance); }; };
	struct vertexData {
		float xPos;// in this texture
		float yPos; //in this texture
		float  width ;//of this glyph in the texture
		float height ;//of this glyph in the texture
		void saveToProtoBuf (sdff::akjFont::vertexData* vertexToSave) {
			vertexToSave->set_xpos (xPos);
			vertexToSave->set_ypos (yPos);
			vertexToSave->set_width (width);
			vertexToSave->set_height (height); }; };



	SignedDistanceFieldFont() {};
	~SignedDistanceFieldFont() {};
	template<class Archive>
	SignedDistanceFieldFont (Archive & ar) {
		ar >> *this; };
	SignedDistanceFieldFont (int width, int height, unsigned int nChars, float scaleFac) : scaleFactor (scaleFac), texHeight (height), texWidth (width), numChars (nChars) {
		chars.reserve(numChars);
		vertices.reserve (nChars); };
	void createFromFile (const std::string& fName) {
		thisFile = fName;
		GOOGLE_PROTOBUF_VERIFY_VERSION;
		int infd = open (thisFile.c_str(),  O_RDWR,
		                  S_IREAD | S_IWRITE);
		google::protobuf::io::FileInputStream base (infd);
		google::protobuf::io::GzipInputStream input (&base, google::protobuf::io::GzipInputStream::ZLIB);
		sdff::akjFont fontToLoad;

		if (!fontToLoad.ParseFromZeroCopyStream (&input)) {
			throw std::runtime_error ("Failed to parse File"); }

		fontName = (fontToLoad.fontname());
		fontFileName = (fontToLoad.fontfilename());
		texHeight = fontToLoad.texheight();
		texWidth = fontToLoad.texwidth();
		offset = fontToLoad.offset();
		scaleFactor = fontToLoad.scalefactor();
		ddsData = fontToLoad.ddsdata();
		numChars = fontToLoad.chars_size();
		chars.clear();
		chars.reserve(numChars);
		charData ch;

		for (unsigned int i = 0; i < numChars; ++i) {
			const sdff::akjFont::charData& chIn = fontToLoad.chars (i);
			ch.ID = chIn.id();
			ch.indx = chIn.indx();
			ch.xOffset = chIn.xoffset();
			ch.yOffset = chIn.yoffset();
			ch.xAdvance = chIn.xadvance();
			chars.push_back (ch); }

		unsigned int numVerts = fontToLoad.vertices_size();
		vertices.clear();
		vertices.reserve (numVerts);
		vertexData v;

		for (unsigned int i = 0; i < numVerts; ++i) {
			const sdff::akjFont::vertexData& vIn = fontToLoad.vertices (i);
			v.xPos = vIn.xpos();
			v.yPos = vIn.ypos();
			v.width = vIn.width();
			v.height = vIn.height();
			vertices.push_back (v); }

		int numKernPairs = fontToLoad.kernpairs_size();
		kernPair k;

		for (int i = 0; i < numKernPairs; ++i) {
			const sdff::akjFont::kernData& kIn = fontToLoad.kernpairs (i);
			k.x = kIn.x();
			k.y = kIn.y();
			k.a = static_cast<short> (kIn.a());
			k.b = static_cast<short> (kIn.b());
			kernPairs.insert (k); } }
	
	bool getKernPair (kernPair &kp) {
		auto kpi = kernPairs.find (kp);

		if (kpi != kernPairs.end()) {
			kp = *kpi;
			return true; }

		return false; }
	float getWidth() const {
		return static_cast<float> (texWidth); };
	float getHeight() const {
		return static_cast<float> (texHeight); };
	void setDDSData (void* ptr, unsigned int numBytes) {
		ddsData.resize (numBytes);
		unsigned char* bytePtr = (unsigned char*) ptr;
		std::for_each (ddsData.begin(), ddsData.end(), [&bytePtr] (char & thisByte) {
			thisByte = *bytePtr;
			++bytePtr; }); };
	const std::string & getFontFileName() const {
		return fontFileName; };
	void setFontFileName (const std::string& fn) {
		fontFileName = fn; };
	void* getVertPtr() const {
		return (void*) &vertices[0]; };
	unsigned int getVertSize() const {
		return sizeof (vertexData); };
	template <class InputIterator>
	void setKernPairs (InputIterator first, InputIterator last) {
		kernPairs.insert (first, last); };
	void freeImageData() {
		ddsData.clear(); };
	void* getImgPtr() const {
		return (void*) &ddsData[0]; };
	unsigned int getImgSize() const {
		return static_cast<unsigned int> (ddsData.size()); };
	void reload() {
		createFromFile (thisFile); };
	unsigned int getNumVerts() const {
		return static_cast<unsigned int> (vertices.size()); };
	unsigned int getNumChars() const {
		return numChars; };
	const vertexData& GetVertex(int i){
		return vertices.at(i);
	}
	float getScale() const {
		return scaleFactor; };
	charData& getCharData (unsigned int charIn) {
		return chars.at (charIn); };
	void addChar (int ID, int indx, float xOffset, float yOffset, float xAdvance) {
		charData cd;
		cd.indx = indx;
		cd.ID = ID;
		cd.xOffset = xOffset;
		cd.yOffset = yOffset;
		cd.xAdvance = xAdvance;
		chars.push_back(cd); };
	void addVertex (float xPos, float yPos, float width, float height) {
		vertexData vd;
		vd.xPos = xPos;
		vd.yPos = yPos;
		vd.width = width;
		vd.height = height;
		vertices.push_back (vd); };
private:

	int offset;
	int texWidth;
	int texHeight;
	unsigned int numChars;
	/* 'unsigned char sdf_data[]' is defined last */
	std::set<kernPair> kernPairs;
	std::string thisFile;
	std::string fontName;
	std::string fontFileName;
	float scaleFactor;
	std::vector<charData> chars;
	std::vector<vertexData> vertices;
	std::string ddsData; };


}
#endif /* HEADER_SIGNED_DISTANCE_FONT_XXX */
