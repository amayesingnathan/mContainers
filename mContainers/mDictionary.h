#pragma once

#include "mList.h"
#include "mDynArray.h"
#include "mBlock.h"

#include "mUtils.h"
#include "mCore.h"

// Hash Table Default Parameters
namespace mContainers {

    // Key and Value type must be default constructable for linked list head
    template<typename Key, typename Val, uint64_t MaxLoad = 1>
    class mDictionary
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
        using Bucket = mList<KeyValPair>;

    private:
        mDynArray<Bucket> mBuckets;
        mDynArray<KeyValPair*> mLinkData;
        uint64_t mSize;
        uint64_t mBucketCount;
        uint64_t mMaxLoad;

    public:
        mDictionary()
            : mBuckets(DEFAULT_BUCKETS), mSize(0), mBucketCount(DEFAULT_BUCKETS), mMaxLoad(MaxLoad) {}

    public: // Access Operators
        Val& operator[](const Key& key)
        {
            Bucket& bucket = mBuckets[Hash(key)]; // Cache bucket for the given key

            if (bucket.size() == 0) return Add(key);
            auto it = bucket.find(key);
            if (it != bucket.end()) return (*it).value;

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
        auto begin() { return mLinkData.begin(); }
        const auto begin() const { return mLinkData.begin(); }
        auto end() { return mLinkData.end(); }
        const auto end() const { return mLinkData.end(); }

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
                if (auto it = mLinkData.find(&key) != bucket.end()) mLinkData.erase(it);
            }

        }

    private: // Underlying Element Modifier Methods
        // This will cause any existing buckets to become invalidated if a rehashing occurs.
        Val& Add(const Key& key)
        {
            if ((mSize / mBucketCount) >= mMaxLoad) ReHash();

            KeyValPair& kv = mBuckets[Hash(key)].emplace_front(key);
            mLinkData.emplace_back(&kv);
            mSize++;

            return kv.value;
        }

        Val& Add(const Key& key, const Val& value)
        {
            if ((mSize / mBucketCount) >= mMaxLoad) ReHash();

            KeyValPair& kv = mBuckets[Hash(key)].emplace_back(key, value);
            mLinkData.emplace_back(&kv);
            mSize++;

            return kv.value;
        }

        template<typename... Args>
        Val& Add(const Key& key, Args&&... args)
        {
            if ((mSize / mBucketCount) >= mMaxLoad) ReHash();

            KeyValPair& kv = mBuckets[Hash(key)].emplace_front(key, std::forward<Args>(args)...);
            mLinkData.emplace_back(&kv);
            mSize++;

            return kv.value;
        }

    private: // Hashing Related Methods
        uint64_t Hash(const Key& key) const
        {
            return Utils::SuperFastHash(key) % mBucketCount;
        }
        uint64_t Hash(const Key* key) const
        {
            assert(key);
            return Utils::SuperFastHash(*key) % mBucketCount;
        }

        void ReHash()
        {
            mBucketCount = Utils::NextPrime(mBucketCount * 2);
            mBlock<Bucket> newBuckets(Memory::Alloc<Bucket>(mBucketCount), mBucketCount, true);
            mBlock<KeyValPair*> newLinkData(Memory::Alloc<KeyValPair*>(mSize), mSize);

            for (const KeyValPair* kv : mLinkData)
            {
                KeyValPair& movedPair = newBuckets[Hash(kv->key)].emplace_front(kv->key, kv->value);
                newLinkData.emplace_back(&movedPair);
            }
            
            mBuckets = newBuckets;
            mLinkData = newLinkData;

        }

    public:
        void printCollisionDist()
        {
            for (uint64_t i = 0; i < mBucketCount; i++)
                std::cout << "Bucket " + std::to_string(i) + ":\t" + std::to_string(mBuckets[i].size()) + "\n";

            std::cout.flush();
        }
    };

}