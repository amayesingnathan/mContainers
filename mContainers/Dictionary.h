#pragma once

#include "List.h"
#include "Vector.h"

#include "Utils.h"

#include <iostream>

// Hash Table Default Parameters
#define DEFAULT_BUCKETS 8
#define LOAD_SCALE      2
#define MAX_BUCKET_SIZE 4

namespace mContainers {
    
    static bool sLimitBucketSize = true;
        
    // Key and Value type must be default constructable for linked list head
    template<typename Key, typename Val, size_t MaxLoad = 1>
    class Dictionary 
    {
    private:
        struct KeyValPair
        {
            const Key key;
            Val value;
            
            KeyValPair() = default;
            KeyValPair(const Key& _key, const Val& _val)
                : key(_key), value(_val) {}
            template<typename... Args>
            KeyValPair(const Key& key, Args&&... valArgs)
                : key(key), value(std::forward<Args>(valArgs)...) {}
            KeyValPair(const KeyValPair&) = default;
            KeyValPair(KeyValPair&&) = default;
                
            bool operator==(const KeyValPair& other) const
            {                
                return key == other.key;
            }
            bool operator!=(const KeyValPair& other) const
            {
                return !(*this == other);
            }
        };

    private:
        using Bucket = List<KeyValPair>;
    
    private:    
        Vector<Bucket> mData;
        Vector<KeyValPair*> mLinkData;
        size_t mSize;
        size_t mBucketCount;
        size_t mMaxLoad;
    
    public:
        Dictionary()
            : mData(DEFAULT_BUCKETS), mSize(0), mBucketCount(DEFAULT_BUCKETS), mMaxLoad(MaxLoad) {}
        
    public: // Access Operators
        Val& operator[](const Key& key)
        {
            Bucket& bucket = mData[Hash(key)]; // Cache bucket for the given key

            if (bucket.size() == 0) return Add(bucket, key).value;
            auto it = bucket.find(key);
            if (it != bucket.end()) return (*it).value;
            
            return Add(bucket, key).value;
        }
        
        const Val& operator[](const Key& key) const
        {
            const Bucket& bucket = mData[Hash(key)]; // Cache bucket for the given key
            
            assert(bucket.size() == 0);
            
            const auto it = bucket.find(key);
            assert(it != bucket.end());
            
            return (*it).value;
        }
        
    public: // Iterator Methods
        auto begin() { return mLinkData.begin(); }
        const auto begin() const { return mLinkData.begin(); }  
        auto end() { return mLinkData.end(); }  
        const auto end() const { return mLinkData.end(); }
        
    public: // Element Modifiers
        KeyValPair& push_back(const Key& key, const Val& val) 
        { return Add(key, val); }
        
        template<typename... Args>
        KeyValPair& emplace_back(const Key& key, Args&&... args)
        { return Add(key, std::forward<Args>(args)...); }
    
        void erase(const Key& key)
        {
            Bucket& bucket = mData[Hash(key)]; // Cache bucket for the given key
            if (bucket.size() == 0) return;
            
            if (auto it = bucket.find(key) != bucket.end()) 
            { 
                bucket.erase(it); 
                mSize--; 
                if (auto it = mLinkData.find(&key) != bucket.end()) mLinkData.erase(it);
            }
            
        }

    private: // Underlying Element Modifier Methods
        // This will cause any existing buckets to become invalidated if a rehashing occurs.
        KeyValPair& Add(Bucket& bucket, const Key& key)
        {
            if (((++mSize / mBucketCount) >= mMaxLoad) ||
                (sLimitBucketSize && bucket.size() == MAX_BUCKET_SIZE)) ReHash();

            KeyValPair& kv = bucket.emplace_back(key);
            mLinkData.emplace_back(&kv);

            return kv;
        }
        KeyValPair& Add(const Key& key)
        {
            return Add(mData[Hash(key)], key);
        }

        KeyValPair& Add(Bucket& bucket, const Key& key, const Val& value)
        {
            if (((++mSize / mBucketCount) >= mMaxLoad) ||
                (sLimitBucketSize && bucket.size() == MAX_BUCKET_SIZE)) ReHash();

            KeyValPair& kv = bucket.emplace_back(key, value);
            mLinkData.emplace_back(&kv);

            return kv;
        }
        KeyValPair& Add(const Key& key, const Val& value)
        {
            return Add(mData[Hash(key)], key, value);
        }

        template<typename... Args>
        KeyValPair& Add(Bucket& bucket, const Key& key, Args&&... args)
        {
            if ((++mSize / mBucketCount) >= mMaxLoad ||
                (sLimitBucketSize && bucket.size() == MAX_BUCKET_SIZE)) ReHash();

            KeyValPair& kv = bucket.emplace_back(key, std::forward<Args>(args)...);
            mLinkData.emplace_back(&kv);

            return kv;
        }
        template<typename... Args>
        KeyValPair& Add(const Key& key, Args&&... args)
        {
            return Add(mData[Hash(key)], key, std::forward<Args>(args)...);
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
            mBucketCount *= LOAD_SCALE;
            Vector<Bucket> newData(mBucketCount);
            
            for (Bucket& bucket : mData)
            {
                for (KeyValPair& kv : bucket)
                    newData[Hash(kv.key)].emplace_back(std::move(kv));
            }
            
            mData = std::move(newData);
        }
        
    public:
        void printCollisionDist()
        {
            for (size_t i = 0; i < mBucketCount; i++)
                std::cout << "Bucket " + std::to_string(i) + ":\t" + std::to_string(mData[i].size()) << std::endl;
        }
    };

}