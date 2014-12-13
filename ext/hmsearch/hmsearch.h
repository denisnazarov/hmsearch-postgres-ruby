/* HmSearch hash lookup library
 *
 * http://commonsmachinery.se/
 * Distributed under an MIT license
 * 
 * Copyright (c) 2014 Commons Machinery
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __HMSEARCH_H_INCLUDED__
#define __HMSEARCH_H_INCLUDED__

#include <string>
#include <list>
#include <stdint.h>

/** Interface to a HmSearch database.
 *
 * It cannot be instantiated directly, instead open() must be used to
 * get a pointer to a database object.
 *
 * The database is closed when the object is deleted.  Until it is closed,
 * any added hashes may not yet have been written to disk.
 *
 * Multiple threads can call HmSearch::insert() and HmSearch::lookup()
 * on the same database object without locks.
 *
 * HmSearch::close() is not thread-safe, so the caller must ensure
 * that no inserts or lookups are in progress.
 *
 * A database file can only be opened by a single process.  This is a
 * limitation in the underlying LevelDB library.
 */
class HmSearch
{
public:
    /** A string for passing around raw (i.e. not hexadecimal) hashes.
     */
    typedef std::basic_string<uint8_t> hash_string;

    /** A record holding a hash found by lookup() and its hamming distance
     * from the query hash.
     */
    struct LookupResult {
        LookupResult(const hash_string& h, int d) : hash(h), distance(d) {}
        hash_string hash;
        int distance;
    };

    typedef std::list<LookupResult> LookupResultList;

    /** Initialise a new hash database file.
     *
     * The database file should not exist, or if it does it must not
     * contain any records.
     *
     * Parameters:
     *
     *  - path:       file path, typically ending in ".ldb"
     *
     *  - hash_bits:  number of bits in the hash (must be a multiple of 8)
     *
     *  - max_error:  maximum hamming distance, must be less than hash_bits
     *
     *  - num_hashes: target number of hashes, used for tuning the database
     *
     *  - error_msg:  if provided, will be set to an string describing any
     *                error, or to an empty string if no error occurred.
     *
     * Returns true if the database could be initialised, false on errors.
     */
    static bool init(const std::string& path, 
                     unsigned hash_bits, unsigned max_error,
                     uint64_t num_hashes,
                     std::string* error_msg = NULL);

    /** Open a database file.
     *
     * The returned object must be deleted when not used any longer to
     * ensure that the database is synced and closed.
     * 
     * Parameters:
     *
     *  - path: file path, typically ending in ".ldb"
     *
     *  - mode: database open mode
     *
     *  - error_msg:  if provided, will be set to an string describing any
     *                error, or to an empty string if no error occurred.
     *
     * Returns the new object on success, or NULL on error.
     */
    static HmSearch* open(const std::string& path,
                          std::string* error_msg = NULL);


    /** Parse a hash in hexadecimal format, returning
     * a string of raw bytes.
     */
    static hash_string parse_hexhash(const std::string& hexhash);

    /** Format a hash of raw bytes into a hexadecimal string.
     */
    static std::string format_hexhash(const hash_string& hash);


    /** Prints a string suitable for PostgreSQL COPY onto stdout.
     *
     * No check is made if the hash already exists in the database,
     * so this may result in duplicate records.
     *
     * Parameters:
     *  - hash:      The hash to print, as raw bytes
     *  - error_msg: if provided, will be set to an string describing any
     *               error, or to an empty string if no error occurred.
     *
     * Returns true if the command succeded, false on any error.
     */
    virtual bool print_copystring(const hash_string& hash,
                        std::string* error_msg = NULL) = 0;

    /** Insert a hash into the database.
     *
     * No check is made if the hash already exists in the database,
     * so this may result in duplicate records.
     *
     * Parameters:
     *  - hash:      The hash to insert, as raw bytes
     *  - error_msg: if provided, will be set to an string describing any
     *               error, or to an empty string if no error occurred.
     *
     * Returns true if the insert succeded, false on any error.
     */
    virtual bool insert(const hash_string& hash,
                        std::string* error_msg = NULL) = 0;

    /** Lookup a hash in the database, returning a list of matches.
     *
     * Parameters:
     *
     *  - query:     query hash string
     *
     *  - result:    matches are added to this list (which is not emptied)
     *
     *  - max_error: if >= 0, reduce the maximum accepted error
     *               from the database default
     *
     *  - error_msg: if provided, will be set to an string describing any
     *               error, or to an empty string if no error occurred.
     *
     * Returns true if the lookup could be performed (even if no
     * hashes were found), false if an error occurred.
     */
    virtual bool lookup(const hash_string& query,
                        LookupResultList& result,
                        int max_error = -1,
                        std::string* error_msg = NULL) = 0;

    /** Explicitly sync and close the database file.
     *
     * Parameter:
     *  - error_msg: if provided, will be set to an string describing any
     *               error, or to an empty string if no error occurred.
     *
     * Returns true if all went well, false on errors.
     */
    virtual bool close(std::string* error_msg = NULL) = 0;

    /** Delete the database object, syncing and closing the database
     * file if not already done.
     */
    virtual ~HmSearch() {}

protected:
    HmSearch() {}
};


/*
  Local Variables:
  c-file-style: "stroustrup"
  indent-tabs-mode:nil
  End:
*/

#endif // __HMSEARCH_H_INCLUDED__

