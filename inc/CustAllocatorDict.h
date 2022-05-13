#pragma once

#include "mCore.h"
#include "mList.h"
#include "mDynArray.h"

#include "mUtils.h"

namespace mContainers {
        
    // Key and Value type must be default constructable for linked list head
    template<typename Key, typename Val, size_t MaxLoad = 1>
    class OldDictionary
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
            size_t index;

            KeyIndexPair() : key(nullptr), index(-1) {}
            KeyIndexPair(const Key& _key, size_t _index)
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
        //using Bucket = std::list<KeyIndexPair>;

        class Bucket
        {
        private:
            KeyIndexPair* mData;
            size_t mSize;

        public:
            Bucket(KeyIndexPair* dataBlock) : mData(dataBlock), mSize(0) {}
            ~Bucket()
            {
                for (size_t i = 0; i < mSize; i++)
                    mData[i].~KeyIndexPair();
                mSize = 0;
            }

            template<typename... Args>
            KeyIndexPair& emplace_back(Args&&... args)
            {
                mAssert(mSize < MAX_BUCKET_SIZE, "Bucket is full!")

                new(&mData[mSize]) KeyIndexPair(std::forward<Args>(args)...);
                return mData[mSize++];
            }

            KeyIndexPair& operator[](size_t index)
            {
                mAssert(index < mSize, "Accessing unitialised entry!");

                return mData[index];
            }
            const KeyIndexPair& operator[](size_t index) const
            {
                mAssert(index < size, "Accessing unitialised entry!");

                return mData[index];
            }

            KeyIndexPair* find(const Key& other)
            {
                for (size_t i = 0; i < mSize; i++)
                    if (mData[i].key == other) return &mData[i];

                return nullptr;
            }

            size_t size() const { return mSize; }
        };
    
        class BucketList
        {
        public:
            BucketList(size_t count = DEFAULT_BUCKETS)
            {
                Build(count);
            }

            ~BucketList()
            {
                Reset();
            }

            Bucket& operator[](size_t index)
            {
                mAssert(index < mBucketCount, "Index out of range!");

                return mBuckets[index];
            }
            const Bucket& operator[](size_t index) const
            {
                mAssert(index < mBucketCount, "Index out of range!");

                return mBuckets[index];
            }

            void resize(size_t count)
            {
                Reset();
                Build(count);
            }

            size_t count() const { return mBucketCount; }

        private:
            void Build(size_t count)
            {
                mBucketCount = count;
                mBuckets = Memory::Alloc<Bucket>(mBucketCount);
                mBase = Memory::Alloc<KeyIndexPair>(mBucketCount * MAX_BUCKET_SIZE);

                KeyIndexPair* bucketBlock = mBase;
                for (size_t i = 0; i < mBucketCount; i++)
                {
                    Memory::Emplace<Bucket>(&mBuckets[i], bucketBlock);
                    bucketBlock += MAX_BUCKET_SIZE;
                }
            }

            void Reset()
            {
                for (size_t i = 0; i < mBucketCount; i++)
                    mBuckets[i].~Bucket();

                Memory::Free<Bucket>(mBuckets, mBucketCount);
                Memory::Free<KeyIndexPair>(mBase, mBucketCount * MAX_BUCKET_SIZE);
            }

        private:
            KeyIndexPair* mBase;
            Bucket* mBuckets;
            size_t mBucketCount;
        };

    private:
        mDynArray<KeyValPair> mData;
        BucketList mBuckets; // Custom Allocator
        size_t mSize;
        size_t mBucketCount;
        size_t mMaxLoad;
    
    public:
        OldDictionary()
            : mSize(0), mBucketCount(DEFAULT_BUCKETS), mMaxLoad(MaxLoad) {}
        
    public: // Access Operators
        Val& operator[](const Key& key)
        {
            Bucket& bucket = mBuckets[Hash(key)]; // Cache bucket for the given key

            if (bucket.size() == 0) return Add(key);

            KeyIndexPair* it = bucket.find(key);
            if (it) return mData[it->index].value;
            
            return Add(key);
        }
        
        const Val& operator[](const Key& key) const
        {
            const Bucket& bucket = mBuckets[Hash(key)]; // Cache bucket for the given key
            
            mAssert(bucket.size() == 0, "Bucket is empty!");
            
            const KeyIndexPair it = bucket.find(key);
            mAssert(it, "Key not in hash table!");
            
            return it->value;
        }
        
    public: // Iterator Methods
        auto begin() { return mData.begin(); }
        const auto begin() const { return mData.begin(); }
        auto end() { return mData.end(); }
        const auto end() const { return mData.end(); }
        
    public: // Element Modifiers
        Val& insert(const Key& key, const Val& val)
        { return Add(key, val); }
        
        template<typename... Args>
        Val& emplace(const Key& key, Args&&... args)
        { return Add(key, std::forward<Args>(args)...); }

    private: // Underlying Element Modifier Methods
        // This will cause any existing buckets to become invalidated if a rehashing occurs.
        Val& Add(const Key& key)
        {
            if ((mSize / mBucketCount) >= mMaxLoad || mBuckets[Hash(key)].size() == MAX_BUCKET_SIZE) ReHash();

            KeyValPair& result = mData.emplace_back(key);
            mBuckets[Hash(key)].emplace_back(result.key, mSize++);

            return result.value;
        }

        Val& Add(const Key& key, const Val& value)
        {
            if ((mSize / mBucketCount) >= mMaxLoad || mBuckets[Hash(key)].size() == MAX_BUCKET_SIZE) ReHash();

            KeyValPair& result = mData.emplace_back(key, value);
            mBuckets[Hash(key)].emplace_back(result.key, mSize++);

            return result.value;
        }

        template<typename... Args>
        Val& Add(const Key& key, Args&&... args)
        {
            if ((mSize / mBucketCount) >= mMaxLoad || mBuckets[Hash(key)].size() == MAX_BUCKET_SIZE) ReHash();

            KeyValPair& result = mData.emplace_back(key, std::forward<Args>(args)...);
            mBuckets[Hash(key)].emplace_back(result.key, mSize++); // Custom Allocator

            return result.value;
        }
        
    private: // Hashing Related Methods
        size_t Hash(const Key& key) const
        {
            return Utils::Hash(key) % mBucketCount;
        }
        size_t Hash(const Key* key) const
        {
            mAssert(key, "Key must not be null!");
            return Utils::Hash(*key) % mBucketCount;
        }
        
        void ReHash() 
        {
            mBucketCount = Utils::NextPrime(mBucketCount * 2);
            mBuckets.resize(mBucketCount);

            for (size_t i = 0; i < mData.size(); i++)
            {
                const Key& key = mData[i].key;
                mBuckets[Hash(key)].emplace_back(key, i);
            }
        }
        
    public:
        void printCollisionDist()
        {
            size_t sum = 0;
            size_t zeroCount = 0;
            std::ostringstream os;
            for (size_t i = 0; i < mBucketCount; i++)
            {
                size_t size = mBuckets[i].size();
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
            size_t sum = 0;
            size_t zeroCount = 0;
            std::ostringstream os;
            for (size_t i = 0; i < mBucketCount; i++)
            {
                size_t size = mBuckets[i].size();
                sum += size;
                if (size == 0) zeroCount++;
                os << std::to_string(i) + " " + std::to_string(size) + "\n";
            }
            std::cout << os.str();
        }
    };

}