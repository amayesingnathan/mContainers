#pragma once

#include "mCore.h"
#include "mList.h"
#include "mDynArray.h"

#include "mUtils.h"

// Hash Table Default Parameters
#define DEFAULT_BUCKETS 7
#define LOAD_SCALE      2
#define MAX_BUCKET_SIZE 4

namespace mContainers {

    static bool sLimitBucketSize = false;

    // Key and Value type must be default constructable for linked list head
    template<typename Key, typename Val, uint64_t MaxLoad = 1>
    class TestDictionary
    {
    private:
        struct KeyValPair
        {
            const Key key;
            Val value;

            KeyValPair() : key(), value() {}
            KeyValPair(const Key& _key, const Val& _val)
                : key(_key), value(_val) {}
            template<typename... Args>
            KeyValPair(const Key& key, Args&&... valArgs)
                : key(key), value(std::forward<Args>(valArgs)...) {}
            KeyValPair(const KeyValPair&) = default;
            KeyValPair(KeyValPair&&) = default;

        };

        struct KeyIndexPair
        {
            const Key& key;
            uint64_t index;

            KeyIndexPair() : key(nullptr), index(-1) {}
            KeyIndexPair(const Key& _key, uint64_t _index)
                : key(_key), index(_index) {}
            KeyIndexPair(const KeyIndexPair&) = default;
            KeyIndexPair(KeyIndexPair&&) = default;

            bool operator==(const KeyIndexPair& other) const
            {
                return key == other.key;
            }
            bool operator!=(const KeyIndexPair& other) const
            {
                return !(*this == other);
            }
        };

    private:
        using Bucket = mList<KeyIndexPair>;

    private:
        mDynArray<Bucket> mBuckets;
        mDynArray<KeyValPair> mData;
        uint64_t mSize;
        uint64_t mBucketCount;
        uint64_t mMaxLoad;

    public:
        TestDictionary()
            : mBuckets(DEFAULT_BUCKETS), mSize(0), mBucketCount(DEFAULT_BUCKETS), mMaxLoad(MaxLoad) {}

    public: // Access Operators
        Val& operator[](const Key& key)
        {
            Bucket& bucket = mBuckets[Hash(key)]; // Cache bucket for the given key

            if (bucket.size() == 0) return Add(key);
            auto it = bucket.find(key);
            if (it != bucket.end()) return mData[(*it).index];

            return Add(key);
        }

        const Val& operator[](const Key& key) const
        {
            const Bucket& bucket = mBuckets[Hash(key)]; // Cache bucket for the given key

            assert(bucket.size() == 0);

            const auto it = bucket.find(key);
            assert(it != bucket.end());

            return (*it).value;
        }

    public: // Iterator Methods
        auto begin() { return mData.begin(); }
        const auto begin() const { return mData.begin(); }
        auto end() { return mData.end(); }
        const auto end() const { return mData.end(); }

    public: // Element Modifiers
        Val& insert(const Key& key, const Val& val)
        {
            return Add(key, val);
        }

        template<typename... Args>
        Val& emplace(const Key& key, Args&&... args)
        {
            return Add(key, std::forward<Args>(args)...);
        }

        void erase(const Key& key)
        {
            Bucket& bucket = mBuckets[Hash(key)]; // Cache bucket for the given key
            if (bucket.size() == 0) return;

            if (auto it = bucket.find(key) != bucket.end())
            {
                bucket.erase(it);
                mSize--;
                if (auto it = mData.find(&key) != bucket.end()) mData.erase(it);
            }

        }

    private: // Underlying Element Modifier Methods
        // This will cause any existing buckets to become invalidated if a rehashing occurs.
        Val& Add(const Key& key)
        {
            if (((mSize / mBucketCount) >= mMaxLoad) ||
                (sLimitBucketSize && mBuckets[Hash(key)].size() == MAX_BUCKET_SIZE)) ReHash();

            KeyValPair& result = mData.emplace_back(key);
            mBuckets[Hash(key)].emplace_front(result.key, mSize++);

            return result.value;
        }

        Val& Add(const Key& key, const Val& value)
        {
            if (((mSize / mBucketCount) >= mMaxLoad) ||
                (sLimitBucketSize && mBuckets[Hash(key)].size() == MAX_BUCKET_SIZE)) ReHash();

            KeyValPair& result = mData.emplace_back(key, value);
            mBuckets[Hash(key)].emplace_front(result.key, mSize++);

            return result.value;
        }

        template<typename... Args>
        Val& Add(const Key& key, Args&&... args)
        {
            if ((mSize / mBucketCount) >= mMaxLoad ||
                (sLimitBucketSize && mBuckets[Hash(key)].size() == MAX_BUCKET_SIZE)) ReHash();

            KeyValPair& result = mData.emplace_back(key, std::forward<Args>(args)...);
            mBuckets[Hash(key)].emplace_front(result.key, mSize++);

            return result.value;
        }

    private: // Hashing Related Methods
        uint64_t Hash(const Key& key) const
        {
            return Utils::Hash(key) % mBucketCount;
        }
        uint64_t Hash(const Key* key) const
        {
            assert(key);
            return Utils::Hash(*key) % mBucketCount;
        }

        void ReHash()
        {
            mBucketCount = Utils::NextPrime(mBucketCount * 2);
            mBuckets.clear();
            mBuckets.resize(mBucketCount);

            for (uint64_t i = 0; i < mData.size(); i++)
            {
                const Key& key = mData[i].key;
                mBuckets[Hash(key)].emplace_front(key, i);
            }
        }

    public:
        void printCollisionDist()
        {
            uint64_t sum = 0;
            uint64_t zeroCount = 0;
            std::ostringstream os;
            for (uint64_t i = 0; i < mBucketCount; i++)
            {
                uint64_t size = mBuckets[i].size();
                sum += size;
                if (size == 0) zeroCount++;
                os << "Bucket " + std::to_string(i) + ":\t" + std::to_string(mBuckets[i].size()) + "\n";
            }
            os << "\n";
            std::cout << os.str();

            std::cout << "Average \t" + std::to_string((float)sum / (float)mBucketCount) + "\n";
            std::cout << "-Zero Avg\t" + std::to_string((float)sum / (float)(mBucketCount - zeroCount)) + "\n";
            std::cout << "Empties \t" + std::to_string(zeroCount) << std::endl;
        }

        void printCollisionDistData()
        {
            uint64_t sum = 0;
            uint64_t zeroCount = 0;
            std::ostringstream os;
            for (uint64_t i = 0; i < mBucketCount; i++)
            {
                uint64_t size = mBuckets[i].size();
                sum += size;
                if (size == 0) zeroCount++;
                os << std::to_string(i) + " " + std::to_string(size) + "\n";
            }
            std::cout << os.str();
        }
    };

}