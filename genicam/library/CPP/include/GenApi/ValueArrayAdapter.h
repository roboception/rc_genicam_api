//-----------------------------------------------------------------------------
//  (c) 2020 by SICK AG
//  Project:  GenApi
//    Author: Mattias Johannesson
//  $Header$
//
//  License: This file is published under the license of the EMVA GenICam  Standard Group.
//  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'.
//  If for some reason you are missing  this file please contact the EMVA or visit the website
//  (http://www.genicam.org) for a full copy.
//
//  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS;
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
/**
\file
\brief    The CValueArrayAdapter class and related utilities.
*/

#ifndef VALUE_ARRAY_ADAPTER_H
#define VALUE_ARRAY_ADAPTER_H

#include <Base/GCException.h>
#include <GenApi/GenApi.h>
#include <GenApi/Synch.h>

#include <algorithm>
#include <exception>
#include <functional>
#include <string>
#include <vector>
#include <iterator>

/* Unified access to byte-swapping tools, possibly candidate to get
 * extracted/extended to a reusable header. */
#ifdef _MSC_VER
  #include <cstdlib>
  #define SW16(s) _byteswap_ushort(s)
  #define SW32(s) _byteswap_ulong(s)
  #define SW64(s) _byteswap_uint64(s)
#elif __APPLE__
  #include <libkern/OSByteOrder.h>
  #define SW16(s) __builtin_bswap16( (s) )
  #define SW32(s) __builtin_bswap32( (s) )
  #define SW64(s) __builtin_bswap64( (s) )
#else
  #include <byteswap.h>
  #define SW16(s) bswap_16(s)
  #define SW32(s) bswap_32(s)
  #define SW64(s) bswap_64(s)
#endif

namespace GENAPI_NAMESPACE
{

  /* Helper mini-template to allow specialized template conversions.
   * Might be reused (or eliminated) in future, for now intended mainly to 
   * implement conversion to bool and numeric types differently.
   * In future we might check if some type limits checking or similar is
   * desirable. */
  template<typename TTargetType>
  class TypeConverter
  {
  public:
    template<typename TSourceType>
    static TTargetType Convert (TSourceType src)
    {
      return static_cast<TTargetType>(src);
    }
  };

  template<>
  class TypeConverter<bool>
  {
  public:
    template<typename TSourceType>
    static bool Convert (TSourceType src)
    {
      return 0 != src;
    }
  };

  /* Helper mini-template to extract byte-swapped values for all numeric
   * types (incl. floats). 
   * Reverse-copy by default, attempt to perform dedicated swaps for 
   * known sizes. */
  template<size_t TValueSize>
  class SwapExtractor
  {
  public:
    static void Extract (const void *src, void *dst)
    {
      std::reverse_copy(reinterpret_cast<const uint8_t*>(src), 
                        reinterpret_cast<const uint8_t*>(src) + TValueSize,
                        reinterpret_cast<uint8_t*>(dst));
    }
  };
  template<>
  class SwapExtractor<8>
  {
  public:
    static void Extract (const void *src, void *dst)
    {
      *reinterpret_cast<uint64_t*>(dst) = SW64(*reinterpret_cast<const uint64_t*>(src));
    }
  };
  template<>
  class SwapExtractor<4>
  {
  public:
    static void Extract (const void *src, void *dst)
    {
      *reinterpret_cast<uint32_t*>(dst) = SW32(*reinterpret_cast<const uint32_t*>(src));
    }
  };
  template<>
  class SwapExtractor<2>
  {
  public:
    static void Extract (const void *src, void *dst)
    {
      *reinterpret_cast<uint16_t*>(dst) = SW16(*reinterpret_cast<const uint16_t*>(src));
    }
  };
  template<>
  class SwapExtractor<1>
  {
  public:
    static void Extract (const void *src, void *dst)
    {
      *reinterpret_cast<uint8_t*>(dst) = *reinterpret_cast<const uint8_t*>(src);
    }
  };

  /**
  \brief Base class wrapping internal implementation details of the value array adapter functionality.

  Not expected to be used directly. Responsible for initialization/preparation tasks
  required by concrete value array adapter classes.
  The nodemap corresponding to the base_value and selector features must stay valid
  throughout the lifetime of the adapter.
  */
  class GENAPI_DECL CValueArrayAdapterBase
  {
  protected:
    CValueArrayAdapterBase(IValue* base_value, IInteger *selector);
    virtual ~CValueArrayAdapterBase ();
  private:
    /* Not copyable */
    CValueArrayAdapterBase(const CValueArrayAdapterBase&);
    CValueArrayAdapterBase& operator =(const CValueArrayAdapterBase&);

  protected:
    /* Retrieves current status of the value array parameters and reads
     * "shadow copy" of the memory holding the array from the port.
     * It is caller's responsibility to apply the lock as needed.
     * Note: the lock is recursive and ReadFromPort() uses only read operations,
     * no write that would trigger callbacks, it is therefore safe to apply
     * the lock from outside. */
    void ReadFromPort ();
    CLock& GetLock() const;

  protected:
    /* Flag indicating successful parse of the dependencies and initialization
     * of the adapter - if false, the adapter cannot be used. */
    bool is_valid;
    /* Static information sniffed from the XML/nodemap at time of creation
     * of the adapter - info about the structure that does not change at runtime.
     * These are guaranteed to be up-to-date once the instance is constructed. */
    bool is_int_reg;
    bool swap_endian;
    bool is_signed;
    bool masked_int;
    uint32_t lsbit;
    uint32_t msbit;
    uint64_t lsbit_mask;
    /* Dynamic values used for every single read. In particular read copy
     * of the register itself.
     * These are updated during each call to ReadFromPort() that should happen
     * just before attempt to extract the actual values. */
    uint64_t current_base_address;
    uint64_t current_reg_length;
    uint64_t current_address_step;
    size_t current_num_values;
    uint8_t *current_array_shadow;

  private:
    /* Implementation details */
    struct ValueArrayInternals;
    ValueArrayInternals* pinternal;
  };

  /**
  \brief Adapter for accessing structured register known to include an array of selector-iterated values.

   Generic worker for any numeric "output" type, typically used through "concretized"
   children, CIntegerValueArray, CFloatValueArray or CBooleanValueArray.
   Intended to significantly improve performance when reading large contiguous arrays
   that would otherwise be read using selector-based iteration.
   To be eligible for use with the adapter, the XML definition of the feature and its selector
   must conform to specific rules that are documented with SFNC feature ValueArrayCandidates
   (and can be tested using member function CheckAdvertisedCompatibility).
   The "effective" value type is expected to be usually same as the "output" value type, 
   with exception of retrieving the bool array as vector<uint8_t> instead of
   the possibly problematic vector<bool>.
  */
  template<typename TValueNodeType, typename TOutputValueType, typename TEffectiveValueType = TOutputValueType>
  class CValueArrayAdapter : public CValueArrayAdapterBase
  {
  public:
    /**
    \brief Creates a CValueArrayAdapter object.
    \param[in]  base_value  The value node holding the array of values (selector iterated).
    \param[in]  selector  The selector node iterating the base_value array.

    Does not throw C++ exceptions, except when memory allocation fails.
    */
    CValueArrayAdapter(TValueNodeType* base_value, IInteger *selector)
      : CValueArrayAdapterBase (base_value, selector),
        current_array_getter(NULL)
    { }

    /**
    \brief Check if the instance is valid and useable.

    The created instance might be invalid in particular if given value/selector feature pair
    is not eligible for use with the adapter (they do not conform to required rules).

    Does not throw C++ exceptions.
    */
    bool IsValid() const
    {
      return is_valid;
    }

    /**
    \brief Get all values of the array.

    Gets all values of the array (for current selector range) as a vector. 
    Returns same results as if the value array was iterated using the corresponding 
    selector - if given value/selector pair is eligible for use with the adapter
    and conforms to the required rules, otherwise the results are undefined. 
    
    The nodemap is expected to advertise the value/selector pairs suitable for use
    with the adapter using the ValueArrayCandidates feature (defined in SFNC).
    The device (nodemap) designer guarantees compatibility with the adapter
    for the advertised features.

    The function does not check access mode of the value feature, user should verify
    its readability when desirable.

    Note that the first run might be little bit slower than following runs, because
    of some initial internal allocations. The data are not cached, the array
    is retrieved fresh on each run.

    Throws C++ exceptions if the operation fails.
    */
    std::vector<TOutputValueType> GetAllValues()
    {
      std::vector<TOutputValueType> values(current_num_values);
      GetAllValues (values);
      return values;
    }

    /**
    \brief Get all values of the array.
    \param[out]  values  The output vector carrying the 'array' values.

    Gets all values of the array (for current selector range) as a vector. 
    Returns same results as if the value array was iterated using the corresponding 
    selector - if given value/selector pair is eligible for use with the adapter
    and conforms to the required rules, otherwise the results are undefined. 
    
    The nodemap is expected to advertise the value/selector pairs suitable for use
    with the adapter using the ValueArrayCandidates feature (defined in SFNC).
    The device (nodemap) designer guarantees compatibility with the adapter
    for the advertised features.

    The function does not check access mode of the value feature, user should verify
    its readability when desirable.

    Note that the first run might be little bit slower than following runs, because
    of some initial internal allocations. The data are not cached, the array
    is retrieved fresh on each run.

    This overload outputs the values in an out-parameter rather than as a return
    value which might offer small performance advantage if the same vector
    is reused across multiple calls (its capacity is reused).

    Throws C++ exceptions if the operation fails.
    */
    void GetAllValues(std::vector<TOutputValueType> &values)
    {
      if (!is_valid)
      {
        throw RUNTIME_EXCEPTION("Invalid value array adapter");
      }

      /* Lock the entire operation, protecting not only against concurrent
       * changes of underlying nodemap parameters, but also against concurrent
       * accessess to the value array adapter itself. */
      AutoLock l(GetLock());

      /* Freeze current state of all the dynamic properties and read the register. */
      PrepareValues ();
      
      assert (current_array_getter != NULL);
      (this->*current_array_getter) (values);
    }

    /**
    \brief Check value/selector feature pair suitability for use with the adapter.
    \param[in]  base_value  The value node holding the array of values (selector iterated).
    \param[in]  selector  The selector node iterating the base_value array.
    \return Returns true if the value/selector pair is advertised to be eligible for use with the adapter.

    The function does not directly touch the value/selector nodes, it rather checks,
    if the nodemap they belong to advertises their compatibility with the adapter
    using the ValueArrayCandidates SFNC feature. It is the device/nodemap designer's
    responsibility to advertise this compatibility only for suitable features.

    Does not throw C++ exceptions.
    */
    static bool CheckAdvertisedCompatibility(TValueNodeType* base_value, IInteger *selector)
    {
      try
      {
        if (!base_value || !selector)
        {
          return false;
        }

        /* The nodes must necessarily belong to the same nodemap */
        INodeMap *node_map = base_value->GetNode()->GetNodeMap();
        if (node_map != selector->GetNode()->GetNodeMap())
        {
          return false;
        }

        /* The set of candidates is per SFNC advertised through string feature
         * ValueArrayCandidates. */
        CStringPtr candidates_node = node_map->GetNode("ValueArrayCandidates");
        if (!candidates_node.IsValid() || !IsReadable(candidates_node))
        {
          return false;
        }
        GENICAM_NAMESPACE::gcstring rawstr = candidates_node->GetValue ();

        /* Erase everything starting from a first occurence of the '#' character.
         * This is a backdoor to allow extending the ValueArrayCandidates format
         * in a backward compatible way. */
        GENICAM_NAMESPACE::gcstring candidates_str = rawstr.substr (0, rawstr.find_first_of("#"));

        /* The list of adapter candidates is encoded (per SFNC) as comma
         * separated list of Value[Selector] entries. */
        GENICAM_NAMESPACE::gcstring_vector tokens;
        Tokenize(candidates_str, tokens, ",");

        /* Finally check if the requested value/selector pair is in the list. */
        GENICAM_NAMESPACE::gcstring wanted = base_value->GetNode()->GetName() 
                                             + "[" + selector->GetNode()->GetName() + "]";
        return tokens.contains (wanted);
      }
      catch (const GenICam::GenericException &)
      {
        return false;
      }
    }
    
  private:
    /* Array getter implementations for individual field types.
     * Implemented as complete array getter rather than just single value getter
     * called from a loop to allow for unrolling-related compiler optimizations. */
    template<typename TFieldType>
    void GetArrayOfFieldValues (std::vector<TOutputValueType> &values)
    {
      assert (current_reg_length!=0 && current_address_step != 0);

      /* Resize the output (note that vector capacity does not change if
       * the original is bigger than needed). */
      values.resize (current_num_values);

      /* Extract the values.
       * The memory read into our shadow copy starts at first desired value
       * (see PrepareValues), therefore we start at offset 0. */
      if (swap_endian)
      {
        ExtractFieldValues_Swap<TFieldType> (current_num_values, 
                                             static_cast<size_t>(current_address_step),
                                             current_array_shadow, &values[0]);
      }
      else
      {
        ExtractFieldValues_Noswap<TFieldType> (current_num_values,
                                               static_cast<size_t>(current_address_step),
                                               current_array_shadow, &values[0]);
      }
    }
    template<typename TFieldType>
    void GetArrayOfFieldBits (std::vector<TOutputValueType> &values)
    {
      assert (current_reg_length!=0 && current_address_step != 0);

      /* Resize the output (note that vector capacity does not change if
       * the original is bigger than needed). */
      values.resize (current_num_values);

      /* Extract the values.
       * The memory read into our shadow copy starts at first desired value
       * (see PrepareValues), therefore we start at offset 0. */
      if (swap_endian)
      {
        ExtractFieldBits_Swap<TFieldType> (current_num_values, 
                                           static_cast<size_t>(current_address_step),
                                           lsbit_mask, lsbit,
                                           current_array_shadow, &values[0]);
      }
      else
      {
        ExtractFieldBits_Noswap<TFieldType> (current_num_values,
                                             static_cast<size_t>(current_address_step),
                                             lsbit_mask, lsbit,
                                             current_array_shadow, &values[0]);
      }
    }
    /* Static workers responsible for the main-loop part of the GetArray-functions.
     * Separating similar workers with code duplication and using all params as 
     * local variables instead of referring to member variables to assist
     * optimizer as much as possible. */
    template<typename TFieldType>
    static void ExtractFieldValues_Noswap(size_t num_values, size_t address_step,
                                          const uint8_t *src, TOutputValueType *dst)
    {
      for (size_t i = 0; i < num_values; ++i)
      {
        const uint8_t *field_ptr = src + i * address_step;

        TFieldType field_value = *reinterpret_cast<const TFieldType*>(field_ptr);
        dst[i] = static_cast<TOutputValueType>(EffectiveValue (field_value));
      }
    }
    template<typename TFieldType>
    static void ExtractFieldValues_Swap(size_t num_values, size_t address_step,
                                        const uint8_t *src, TOutputValueType *dst)
    {
      for (size_t i = 0; i < num_values; ++i)
      {
        const uint8_t *field_ptr = src + i * address_step;

        TFieldType field_value = ReadSwapped<TFieldType> (field_ptr);
        dst[i] = static_cast<TOutputValueType>(EffectiveValue (field_value));
      }
    }
    template<typename TFieldType>
    static void ExtractFieldBits_Noswap(size_t num_values, size_t address_step,
                                        uint64_t mask, size_t shift,
                                        const uint8_t *src, TOutputValueType *dst)
    {
      for (size_t i = 0; i < num_values; ++i)
      {
        const uint8_t *field_ptr = src + i * address_step;

        TFieldType field_value = *reinterpret_cast<const TFieldType*>(field_ptr);
        uint64_t field_bits = mask & (field_value >> shift);
        dst[i] = static_cast<TOutputValueType>(EffectiveValue (field_bits));
      }
    }
    template<typename TFieldType>
    static void ExtractFieldBits_Swap(size_t num_values, size_t address_step,
                                      uint64_t mask, size_t shift,
                                      const uint8_t *src, TOutputValueType *dst)
    {
      for (size_t i = 0; i < num_values; ++i)
      {
        const uint8_t *field_ptr = src + i * address_step;

        TFieldType field_value = ReadSwapped<TFieldType> (field_ptr);
        uint64_t field_bits = mask & (field_value >> shift);
        dst[i] = static_cast<TOutputValueType>(EffectiveValue (field_bits));
      }
    }

    /* Common worker for the public read-functions. 
     * Prepares for effective value-by-value read - computes current values
     * of the possibly dynamic parameters, reads from the register into 
     * the shadow copy. */
    void PrepareValues()
    {
      /* Scan the current dependencies and read the underlying memory of the
       * value array from the port. Ready for extraction.
       * This updates all the array "coordinates" (address/step/etc.) and
       * gets the memory blob from which it can be extracted. */
      ReadFromPort ();

      /* Finally, knowing all parameters about the array, we can construct
       * the value getter function. */
      if (is_int_reg)
      {
        /* Integer register */
        if (masked_int)
        {
          /* Masked integer value.
           * Supported only with unsigned MaskedIntReg registers, signed refused during consruction. */
          if (is_signed)
          {
            assert (false && "Signed MaskeIntReg should be refused in constructor");
            throw RUNTIME_EXCEPTION("Unsupported signed masked integer register");
          }

          /* Unsigned integer */
          switch (current_reg_length)
          {
          case 8:
            current_array_getter = &CValueArrayAdapter::GetArrayOfFieldBits<uint64_t>;
            break;

          case 4:
            current_array_getter = &CValueArrayAdapter::GetArrayOfFieldBits<uint32_t>;
            break;

          case 2:
            current_array_getter = &CValueArrayAdapter::GetArrayOfFieldBits<uint16_t>;
            break;

          case 1:
            current_array_getter = &CValueArrayAdapter::GetArrayOfFieldBits<uint8_t>;
            break;

          default:
              throw RUNTIME_EXCEPTION("Unsupported unsigned integer register length");
          }
        } /* if (masked_int) */
        else
        {
          /* Full (not masked) integer value */
          if (is_signed)
          {
            /* Signed integer */
            switch (current_reg_length)
            {
            case 8:
              current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<int64_t>;
              break;

            case 4:
              current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<int32_t>;
              break;

            case 2:
              current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<int16_t>;
              break;

            case 1:
              current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<int8_t>;
              break;

            default:
                throw RUNTIME_EXCEPTION("Unsupported signed integer register length");
            }
          } /* if (signed) */
          else
          {
            /* Unsigned integer */
            switch (current_reg_length)
            {
            case 8:
              current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<uint64_t>;
              break;

            case 4:
              current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<uint32_t>;
              break;

            case 2:
              current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<uint16_t>;
              break;

            case 1:
              current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<uint8_t>;
              break;

            default:
                throw RUNTIME_EXCEPTION("Unsupported unsigned integer register length");
            }
          } /* if-else (signed) */
        } /* if-else (masked_int) */
      } /* if (is_int_reg) */
      else
      {
        /* Float register. */
        switch (current_reg_length)
        {
        case 8:
          current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<float64_t>;
          break;

        case 4:
          current_array_getter = &CValueArrayAdapter::GetArrayOfFieldValues<float32_t>;
          break;

        default:
            throw RUNTIME_EXCEPTION("Unsupported float register length");
        }
      } /* if-else (is_int_reg) */
    }

    template<typename TRawType>
    static TEffectiveValueType EffectiveValue (TRawType raw_value)
    {
      return TypeConverter<TEffectiveValueType>::Convert(raw_value);
    }

    template<typename TFieldType>
    static TFieldType ReadSwapped (const void *ptr)
    {
      TFieldType result;
      SwapExtractor<sizeof(TFieldType)>::Extract (ptr, &result);
      return result;
    }

  private:
    typedef void (CValueArrayAdapter::*ArrayGetterFunction)(std::vector<TOutputValueType> &values);
    ArrayGetterFunction current_array_getter;
  };


  /**
  \brief Concrete value array implementation to be used with IInteger based target value nodes.

   The value is presented as int64_t, same as by IInteger itself.
  */
  class CIntegerValueArray : public CValueArrayAdapter<IInteger, int64_t>
  {
  public:
    CIntegerValueArray(IInteger *base_value, IInteger *selector)
      : CValueArrayAdapter<IInteger, int64_t> (base_value, selector)
    {
    }
  };

  /**
  \brief Concrete value array implementation to be used with IFloat based target value nodes.

   The value is presented as double, same as by IFloat itself.
  */
  class CFloatValueArray : public CValueArrayAdapter<IFloat, double>
  {
  public:
    CFloatValueArray(IFloat *base_value, IInteger *selector)
      : CValueArrayAdapter<IFloat, double> (base_value, selector)
    {
    }
  };

  /**
  \brief Concrete value array implementation to be used with IBoolean based target value nodes.

   The value is presented as uint8_t, contrary to IBoolean itself,
   to avoid possible problems with vector<bool>.
  */
  class CBooleanValueArray : public CValueArrayAdapter<IBoolean, uint8_t, bool>
  {
  public:
    CBooleanValueArray(IBoolean *base_value, IInteger *selector)
      : CValueArrayAdapter<IBoolean, uint8_t, bool> (base_value, selector)
    {
    }
  };

}

#endif // VALUE_ARRAY_ADAPTER_H

