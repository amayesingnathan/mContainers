#pragma once

#include "mCore.h"
#include "mList.h"
#include "mDynArray.h"
#include "mBlock.h"

#include "mUtils.h"

// Hash Table Default Parameters
#define DEFAULT_BUCKETS 7
#define LOAD_SCALE      2
#define MAX_BUCKET_SIZE 10

namespace mContainers {
        
    // Key and Value type must be default constructable for linked list head
    template<typename Key, typename Val, size_t MaxLoad = 1>
    class Dictionary 
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
        using Bucket = mBlock<KeyIndexPair>;
    
        class BucketList : public mDynArray<Bucket>
        {
        public:
            BucketList(size_t count)
            {
                this->ReAllocConstruct(count);
                KeyIndexPair* newBlock = reinterpret_cast<KeyIndexPair*>(mAlloc(count * MAX_BUCKET_SIZE * sizeof(KeyIndexPair)));

                KeyIndexPair* bucketBlock = newBlock;
                for (size_t i = 0; i < count; i++)
                {
                    new(&this->mData[i]) Bucket(bucketBlock, MAX_BUCKET_SIZE);
                    bucketBlock += MAX_BUCKET_SIZE;
                }
                mRoot = newBlock;
            }

            ~BucketList()
            {
                this->clear();
                mFree(mRoot, this->mSize * MAX_BUCKET_SIZE * sizeof(KeyIndexPair));
            }

            void resize(size_t count) override
            {
                for (size_t i = 0; i < this->mSize; i++)
                    this->mData[i].~Bucket();
                mFree(mRoot, this->mSize * MAX_BUCKET_SIZE * sizeof(KeyIndexPair));
                this->mSize = 0;
                this->ReAllocConstruct(count);

                KeyIndexPair* newBlock = reinterpret_cast<KeyIndexPair*>(mAlloc(count * MAX_BUCKET_SIZE * sizeof(KeyIndexPair)));
                KeyIndexPair* bucketBlock = newBlock;

                for (size_t i = 0; i < count; i++)
                {
                    mPlace<Bucket>(&this->mData[i], bucketBlock, MAX_BUCKET_SIZE);
                    bucketBlock += MAX_BUCKET_SIZE;
                }
                mRoot = newBlock;
            }

        private:
            KeyIndexPair* mRoot;
        };

    private:    
        BucketList mBuckets;
        mDynArray<KeyValPair> mData;
        size_t mSize;
        size_t mBucketCount;
        size_t mMaxLoad;
    
    public:
        Dictionary()
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
        { return Add(key, val); }
        
        template<typename... Args>
        Val& emplace(const Key& key, Args&&... args)
        { return Add(key, std::forward<Args>(args)...); }
    
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
            if ((mSize / mBucketCount) >= mMaxLoad || mBuckets[Hash(key)].size() == MAX_BUCKET_SIZE) ReHash();

            KeyValPair& result = mData.emplace_back(key);
            mBuckets[Hash(key)].emplace_front(result.key, mSize++);

            return result.value;
        }

        Val& Add(const Key& key, const Val& value)
        {
            if ((mSize / mBucketCount) >= mMaxLoad || mBuckets[Hash(key)].size() == MAX_BUCKET_SIZE) ReHash();

            KeyValPair& result = mData.emplace_back(key, value);
            mBuckets[Hash(key)].emplace_front(result.key, mSize++);

            return result.value;
        }

        template<typename... Args>
        Val& Add(const Key& key, Args&&... args)
        {
            if ((mSize / mBucketCount) >= mMaxLoad || mBuckets[Hash(key)].size() == MAX_BUCKET_SIZE) ReHash();

            KeyValPair& result = mData.emplace_back(key, std::forward<Args>(args)...);
            mBuckets[Hash(key)].emplace_back(result.key, mSize++);

            return result.value;
        }
        
    private: // Hashing Related Methods
        size_t Hash(const Key& key) const
        {
            return Utils::SuperFastHash(key) % mBucketCount;
        }
        size_t Hash(const Key* key) const
        {
            assert(key);            
            return Utils::SuperFastHash(*key) % mBucketCount;
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